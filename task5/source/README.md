# 説明

ABCアルゴリズムを用いてHP構造予測問題を解くプログラムを作成しました．

## パラメータ

```src/main.cpp``` 内を編集することでパラメーターを変更できます.

```c++
11  #define INPUT " PHPH2P2HPH3P2H2PH2P3H5P2HPH2(PH)2P4HP2(HP)2";
12  #define N 2000         
13  #define S_limit 50    
14  #define GEN_MAX 30 
```

各パラメーターの定義

- `INPUT`:  
  HPモデルの入力配列を定義する文字列． 
  - `H`: 疎水性アミノ酸  
  - `P`: 親水性アミノ酸

- `N`:  
  Employed bee の個体数

- `S_limit`:  
  Employed bee の探索回数の上限．

- `GEN_MAX`:  
  世代数

## コンパイル

```ABC_2D```，```ABC_3D``` はそれぞれ，

```bash
$ make
```

でコンパイルできます．このプログラムでは結果の描画に OpenGL の GLUT を用いています．GLUTは

```bash
$ sudo apt-get install freeglut3 freeglut3-dev
```

でインストールできます．また，このプログラムは Ubuntu上で作成しているので，Mac環境の場合はOpenGLでエラーが出るかもしれませんが，その場合は```src/display.cpp```の

```c++
8   #include <GL/glut.h>
```

を

```c++
8   #include <GLUT/glut.h>
```

にするとたぶんコンパイルできます．

## 実行

コンパイル出来たら

```bash
$ bin/main
```

で実行できます．入力配列が不適切な場合は，以下のようにエラーになります．

```bash

  HPH2Pa2H4PH3P2H2P2HPH2PHPH2P2H2P3HP8H2
       ^
syntax error 
```

適切な配列を入力すると次のようにシミュレーションが開始され，現在の世代数が表示されます．

```bash

  (HP)2PH2PHP2HPH2P2HPH  

*****Simulation start*******
 Generation : 7
```

世代数が ```GEN_MAX``` に到達するとシミュレーションが終了し，終了状態の構造群の中の水素結合の最大値と実行時間が次のように表示されます．

```bash

  (HP)2PH2PHP2HPH2P2HPH  

*****Simulation finish*******
 Generation : 100
 Maximum hydrogen bonds found: 11
 Duration time : 0.337332 s
```

また，最大の水素結合数を与える構造を次のように OpenGL で表示します．

### ディスプレイ操作方法

- 赤色の球 : 疎水性アミノ酸，青色の球 : 親水性アミノ酸
- 赤色の線 : ペプチド結合，青色の点線 : 水素結合
- 左クリックをしながらマウスを動かすと視点を変更
- 右クリックをしながらマウスを動かすと画面を拡大縮小
- 画面右上の X マークをクリックすると画面が閉じてプログラムが終了
  
## プログラムの説明



- ```vec3.h``` : 3次元ベクトルのクラス
- ```display.h```, ```display.cpp``` : OpenGLでの描画
- ```decode.h```, ```decode.cpp``` : 入力配列を処理しやすい形に変形
  
  例：

  ```
  (HP)2PH2PHP2HPH2P2HPH -> HPHPPHHPHPPHPHHPPHPH
  ```

- ```main.c``` : main関数