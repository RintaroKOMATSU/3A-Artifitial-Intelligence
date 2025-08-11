#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <numeric>
#include "vec3.h"
#include "display.h"
#include <GL/glut.h>

#define WINDOW_X (1000)
#define WINDOW_Y (1000)
#define WINDOW_NAME "Result"

#define P_COLOR {10, 0.5, 1}
#define H_COLOR {180, 0.5, 1}
#define CN_COLOR {10, 0.5, 1}
#define HH_COLOR {180, 0.5, 0.5}

static void init_GL(int argc, char *argv[]);
static void init();
static void set_callback_functions();

static void glut_display();
static void glut_mouse(int button, int state, int x, int y);
static void glut_motion(int x, int y); 

static void draw_sphere(const vec3 pos, const double radius, double* color);
static void draw_cylinder(const vec3 start, const vec3 end, const double radius, double* color);
static void draw_dashed_cylinder(const vec3 start, const vec3 end, const double radius, double* color);

// 初期状態で斜め45度から見る回転行列（X軸周りに30度、Y軸周りに45度回転）
static double g_rotation_matrix[16] = {
    0.707, 0.5, -0.5, 0,      // cos(45°), sin(45°)*sin(30°), -sin(45°)*cos(30°), 0
    0, 0.866, 0.5, 0,         // 0, cos(30°), sin(30°), 0  
    0.707, -0.354, 0.612, 0,  // cos(45°), -sin(45°)*sin(30°), sin(45°)*cos(30°), 0
    0, 0, 0, 1                // 0, 0, 0, 1
};
static double g_distance = 20.0;
static bool g_isLeftButtonOn = false;
static bool g_isRightButtonOn = false;

static std::string input;
static std::vector<vec3> pos_arr;

int display(const std::string& input0, const std::vector<vec3>& pos_arr0) {
	init_GL(1, nullptr);
        
    input = input0;
    pos_arr = pos_arr0;

	init();
	set_callback_functions();
	glutMainLoop(); 

	return 0;
}


static void init_GL(int argc, char *argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_X,WINDOW_Y);
	glutCreateWindow(WINDOW_NAME);
}

static void init(){
	glClearColor(0.0, 0.0, 0.0, 0.0);         // 背景の塗りつぶし色を指定
	double max_x = 0;
	double min_x = 0;
	double max_y = 0;
	double min_y = 0;
	double max_z = 0;
	double min_z = 0;
	for (std::size_t i = 0; i < input.size(); i++) {
		max_x = std::fmax(max_x, pos_arr[i].x());
		min_x = std::fmin(min_x, pos_arr[i].x());
		max_y = std::fmax(max_y, pos_arr[i].y());
		min_y = std::fmin(min_y, pos_arr[i].y());
		max_z = std::fmax(max_z, pos_arr[i].z());
		min_z = std::fmin(min_z, pos_arr[i].z());
	}
	double max_width = std::fmax(max_x-min_x,std::fmax(max_y-min_y, max_z-min_z));
	g_distance = max_width*2.5;
}

static void set_callback_functions(){
	glutDisplayFunc(glut_display);
	glutMouseFunc(glut_mouse);
	glutMotionFunc(glut_motion);
	glutPassiveMotionFunc(glut_motion);
}


static void glut_mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_UP){
			g_isLeftButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isLeftButtonOn = true;
		}
	}

	if(button == GLUT_RIGHT_BUTTON){
		if(state == GLUT_UP){
			g_isRightButtonOn = false;
		}else if(state == GLUT_DOWN){
			g_isRightButtonOn = true;
		}
	}

	// マウスホイールの処理（GLUTではボタン3と4として処理される）
	if(state == GLUT_DOWN){
		if(button == 3){  // ホイールアップ（拡大）
			g_distance = std::fmax(g_distance - 1.0, 1.0);
			glutPostRedisplay();
		}else if(button == 4){  // ホイールダウン（縮小）
			g_distance += 1.0;
			glutPostRedisplay();
		}
	}
}

// トラックボール用のヘルパー関数
static vec3 get_arcball_vector(int x, int y) {
    vec3 P = {
        (2.0 * x - WINDOW_X) / WINDOW_X,
        (WINDOW_Y - 2.0 * y) / WINDOW_Y,
        0
    };
    
    double OP_squared = P.x() * P.x() + P.y() * P.y();
    if (OP_squared <= 1) {
        P = vec3(P.x(), P.y(), sqrt(1 - OP_squared));
    } else {
        P.normalize();
    }
    return P;
}

static void multiply_matrix(double* result, const double* a, const double* b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
}

static void glut_motion(int x, int y){
	static int px = -1, py = -1;
	if(g_isLeftButtonOn == true){
		if(px >= 0 && py >= 0){
			// トラックボール回転
			vec3 va = get_arcball_vector(px, py);
			vec3 vb = get_arcball_vector(x, y);
			
			double angle = acos(fmin(1.0, dot(va, vb)));
			vec3 axis = cross(va, vb);
			
			if (axis.length() > 0.001) {
				axis.normalize();
				
				// 回転行列を作成
				double cos_angle = cos(angle);
				double sin_angle = sin(angle);
				double rotation[16] = {
					cos_angle + axis.x() * axis.x() * (1 - cos_angle),
					axis.x() * axis.y() * (1 - cos_angle) - axis.z() * sin_angle,
					axis.x() * axis.z() * (1 - cos_angle) + axis.y() * sin_angle,
					0,
					axis.y() * axis.x() * (1 - cos_angle) + axis.z() * sin_angle,
					cos_angle + axis.y() * axis.y() * (1 - cos_angle),
					axis.y() * axis.z() * (1 - cos_angle) - axis.x() * sin_angle,
					0,
					axis.z() * axis.x() * (1 - cos_angle) - axis.y() * sin_angle,
					axis.z() * axis.y() * (1 - cos_angle) + axis.x() * sin_angle,
					cos_angle + axis.z() * axis.z() * (1 - cos_angle),
					0,
					0, 0, 0, 1
				};
				
				// 現在の回転行列と新しい回転を合成
				double new_matrix[16];
				multiply_matrix(new_matrix, rotation, g_rotation_matrix);
				for (int i = 0; i < 16; i++) {
					g_rotation_matrix[i] = new_matrix[i];
				}
			}
		}
		px = x;
		py = y;
	}else if(g_isRightButtonOn == true){
		if(px >= 0 && py >= 0){
			// 平行投影用のズーム：表示サイズを変更
			g_distance = std::fmax(g_distance + (double)(y - py)/20, 1.0);
		}
		px = x;
		py = y;
	}else{
		px = -1;
		py = -1;
	}
	glutPostRedisplay();
}

static void glut_display(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	double ortho_size = g_distance * tan(30.0 * M_PI / 180.0);  // 透視投影と同等のサイズ
	glOrtho(-ortho_size, ortho_size, -ortho_size, ortho_size, 0.1, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// 固定カメラ位置からオブジェクトを見る（平行投影では距離は表示サイズに影響しない）
	gluLookAt(0, 0, g_distance, 0, 0, 0, 0, 1, 0);
    
	GLfloat light_pos0[] = {100 , 100, 100, 1};
    GLfloat light_color0[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_ambient0[] = {0.5, 0.5, 0.5, 1.0};
	GLfloat light_pos1[] = {-100 , -100, -100, 1};
	GLfloat light_color1[] = {0.5, 0.5, 0.5, 1.0};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT0 ,GL_SPECULAR, light_ambient0);
    glLightfv(GL_LIGHT0 ,GL_DIFFUSE, light_color0);

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
    glLightfv(GL_LIGHT1 ,GL_DIFFUSE, light_color1);
    vec3 g_center = std::accumulate(pos_arr.begin(), pos_arr.end(), vec3(0, 0, 0))/pos_arr.size();
	
	// OpenGL用に転置した回転行列を作成
	double gl_matrix[16] = {
		g_rotation_matrix[0], g_rotation_matrix[4], g_rotation_matrix[8], g_rotation_matrix[12],
		g_rotation_matrix[1], g_rotation_matrix[5], g_rotation_matrix[9], g_rotation_matrix[13],
		g_rotation_matrix[2], g_rotation_matrix[6], g_rotation_matrix[10], g_rotation_matrix[14],
		g_rotation_matrix[3], g_rotation_matrix[7], g_rotation_matrix[11], g_rotation_matrix[15]
	};
	
	// 構造の中心を画面中央で回転させる
	// 1. 回転を適用
	glMultMatrixd(gl_matrix);
	// 2. 構造の中心を原点に移動
	glTranslatef(-g_center[0], -g_center[1], -g_center[2]);
    for (std::size_t i = 0; i < input.size(); i++) {
        if (input[i] == 'H') {
            double color[3] = H_COLOR;
			draw_sphere(pos_arr[i], 0.2, color);
        } else {
            double color[3] = P_COLOR;
			draw_sphere(pos_arr[i], 0.2, color);
        }
    }
    for (std::size_t i = 0; i < input.size()-1; i++) {
        double color[3] = CN_COLOR;
        draw_cylinder(pos_arr[i], pos_arr[i+1], 0.05, color);
    }
	for (std::size_t i = 2; i < input.size(); i++) {
		for (std::size_t j = 0; j < i-1; j++) {
			if (input[i] == 'H' && input[j] == 'H' && distance(pos_arr[i], pos_arr[j])==1) {
				double color[3] = HH_COLOR;
				draw_dashed_cylinder(pos_arr[i], pos_arr[j], 0.015, color);
			}
		}
	}
	glFlush();
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

static void hsv2rgb(double* hsv, double* rgb) {
	double h = hsv[0], s = hsv[1], v = hsv[2];
	double h0 = (fmod(h, 360))/60;
	double c = s;
	double x = c*(1-abs(fmod(h0, 2) -1));
	double r = v-c, g = v-c, b = v-c;
	switch (((int)h0))
	{
	case 0:
		r += c;g+=x;
		break;
	case 1:
		r +=x;g+=c;
		break;
	case 2:
		g +=c;b+=x;
		break;
	case 3:
		g+=x;b+=c;
		break;
	case 4:
		r+=x;b+=c;
		break;
	case 5:
		r+=c;b+=x;
		break;
	}
	rgb[0] = r; rgb[1] = g; rgb[2] = b;
}



static void draw_sphere(const vec3 pos, const double radius, double* color) {
    glPushMatrix();
	double rgb[3];
	hsv2rgb(color, rgb);
	GLfloat facecolor[] = {(GLfloat)rgb[0], (GLfloat)rgb[1], (GLfloat)rgb[2], 1.0f};
	GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, specular);

    glTranslatef(pos[0], pos[1], pos[2]);
    glutSolidSphere(radius,16,16);
    glPopMatrix();
}

static void draw_cylinder(const vec3 start, const vec3 end, const double radius, double* color) {
    glPushMatrix();
	double rgb[3];
	hsv2rgb(color, rgb);
	GLfloat facecolor[] = {(GLfloat)rgb[0], (GLfloat)rgb[1], (GLfloat)rgb[2], 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, facecolor);

    vec3 axis = end-start;
	double height = axis.length();
	if (axis == vec3(0, 0, 0)) {
		glPopMatrix();
		return;
	}
    axis.normalize();

    vec3 zAxis = {0.0, 0.0, 1.0};
    double angle = std::acos(axis[2]) * 180.0 / M_PI;

    vec3 rotAxis = {
        zAxis[1] * axis[2] - zAxis[2] * axis[1],
        zAxis[2] * axis[0] - zAxis[0] * axis[2],
        zAxis[0] * axis[1] - zAxis[1] * axis[0]
    };

    double rotAxisLength = rotAxis.length();
    if (rotAxisLength > 1e-6) {
        rotAxis = {rotAxis[0] / rotAxisLength, rotAxis[1] / rotAxisLength, rotAxis[2] / rotAxisLength};
    } else {
        rotAxis = {1.0, 0.0, 0.0};
    }


    glTranslatef(start[0], start[1], start[2]);
    glRotatef(angle, rotAxis[0], rotAxis[1], rotAxis[2]);

    GLUquadric* quad = gluNewQuadric();
    gluDisk(quad, 0, radius, 16, 1);

    gluCylinder(quad, radius, radius, height, 16, 16);

    glPushMatrix();
    glTranslatef(0, 0, height);
    gluDisk(quad, 0, radius, 32, 1);
    glPopMatrix();


    gluDeleteQuadric(quad);
    glPopMatrix();
}



static void draw_dashed_cylinder(const vec3 start, const vec3 end, const double radius, double* color) {
	vec3 dir = (end - start).normalize();
	vec3 tmp = start;
	while (distance(tmp, start) < distance(end, start)) {
		draw_cylinder(tmp,tmp + 0.08*dir, radius, color);
		tmp += 0.15*dir;
	}
}