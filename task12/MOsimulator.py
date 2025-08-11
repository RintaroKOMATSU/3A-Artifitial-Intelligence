import customtkinter as ctk
import tkinter as tk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import matplotlib.pyplot as plt
import numpy as np
import re
import copy
from itertools import product
import warnings


warnings.filterwarnings("error", category=RuntimeWarning)
ctk.set_default_color_theme("blue") 
ctk.set_appearance_mode("dark")

class MOsimulator:
    def __init__(self, master):
        self.mode_abc = False
        self.mode_gwo = False
        self.population_size = 0
        self.EA_count = 0
        
        self.running = False
        self.sim_start = False
        
        self.variable_max = []
        self.variable_min = []
        self.variable_count = 0
        self.subfunc_count = 0
        self.func1 = ""
        self.func2 = ""
        self.func = None
        
        self.pareto_optimal = False
        self.pareto_min = []
        self.pareto_max = []
        self.pareto_func1 = ""
        self.pareto_func2 = ""
        self.pareto_func = None
        
        self.moabc = None

        
        self.master = master
        self.master.config(bg = "#000000")
        self.master.title("Multi-objective Optimization Simulator")
        
        #*****************************メニュー設定*************************************
        menu_bar = tk.Menu(master, relief = "flat", bg = "#000000", fg = "white", activebackground = "#444444", activeforeground="white")
        master.config(menu=menu_bar)
        #ファイルメニュー
        file_menu = tk.Menu(menu_bar, tearoff=False, relief = "flat", bg = "#000000", fg = "white", activebackground = "#444444", activeforeground="white")
        menu_bar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="Load Function Setting", command=self.load_function)
        file_menu.add_command(label="Save Function Setting", command = self.save_function)
        file_menu.add_command(label="Save Figure", command=self.save_figure)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.master.quit)

        #シミュレーションメニュー
        sim_menu = tk.Menu(menu_bar, tearoff=False, relief = "flat", bg = "#000000", fg = "white", activebackground = "#444444", activeforeground="white")
        menu_bar.add_cascade(label="Simulation", menu=sim_menu)
        sim_menu.add_command(label="Start Simulation", command=self.start_simulation)
        sim_menu.add_command(label="Pose Simulation", command=self.stop_simulation)
        sim_menu.add_separator()
        sim_menu.add_command(label="Show CR", command=self.show_CR)
        sim_menu.add_command(label="Show RNI", command=self.show_RNI)
        


        
        #*****************************グラフ設定*************************************
        self.fig_frame = ctk.CTkFrame(self.master, fg_color= "#212121", bg_color="#000000")
        self.fig_frame.pack(side = ctk.LEFT, fill=ctk.Y, padx=5, pady=5, expand = True)
        plt.rcParams.update({'text.color': 'white', 
                     'axes.labelcolor': 'white', 
                     'xtick.color': 'white', 
                     'ytick.color': 'white', 
                     'axes.titlecolor': 'white'})
        
        self.fig = plt.Figure(figsize=(7, 5), dpi = 140)
        self.fig.patch.set_alpha(0) 
        self.ax = self.fig.add_subplot(111)
        self.ax.patch.set_facecolor('#444444')
        self.ax.set_xlabel(r"$f_1$", labelpad=10, fontsize = 14)
        self.ax.set_ylabel(r"$f_2$", labelpad=10, fontsize = 14)
        self.ax.xaxis.set_label_coords(0.5, -0.1)
        self.ax.yaxis.set_label_coords(-0.1, 0.5)
        self.ax.set_title("Generation :   ")
        self.ax.grid(color='gray', linestyle='--', linewidth=0.5)
        
        self.x_data_abc = np.array([])
        self.y_data_abc = np.array([])
        self.x_data_gwo = np.array([])
        self.y_data_gwo = np.array([])
        
        self.x_data_theory = np.array([])
        self.y_data_theory = np.array([])
        
        self.plot,  = self.ax.plot(self.x_data_theory, self.y_data_theory, c = 'hotpink', ls = '--', lw = 0.7, zorder=1)
    
        self.scatter_abc = self.ax.scatter(self.x_data_abc, self.y_data_abc, c='royalblue', s=10, zorder=2, edgecolor = "whitesmoke", linewidth = 0.5, label="MOABC")
        self.scatter_gwo = self.ax.scatter(self.x_data_gwo, self.y_data_gwo, marker = 'd', c='lawngreen', s=10, zorder=2, edgecolor = "whitesmoke", linewidth = 0.5, label="MOGWO")
        

        self.canvas = FigureCanvasTkAgg(self.fig, master=self.fig_frame)
        self.canvas_widget = self.canvas.get_tk_widget()
        self.canvas_widget.configure(bg= "#212121", highlightthickness=0) 
        self.canvas_widget.pack(expand=True, pady=10, padx=20)
    
        self.param_frame = ctk.CTkFrame(master, fg_color= "#000000", bg_color="#000000")
        self.param_frame.pack(side = ctk.RIGHT,  fill=ctk.Y, expand = False)
        #****************************シミュレーション*******************************
        button_frame = ctk.CTkFrame(self.param_frame, fg_color= "#292929")
        button_frame.pack(fill=ctk.X, pady=5, side=ctk.BOTTOM, padx = 5)
        

        #開始ボタン
        self.start_button = ctk.CTkButton(button_frame, text="●", command=self.start_simulation, border_color= "gray", fg_color= "#292929", border_width=2, hover_color= "#414141", width = 100)
        self.start_button.pack(side=ctk.LEFT, padx=10, pady = 10)
        #停止ボタン
        self.stop_button = ctk.CTkButton(button_frame, text="▮▮", command=self.stop_simulation, border_color= "gray", fg_color= "#414141", border_width=2, hover_color= "#414141", width = 100, text_color="gray")
        self.stop_button.pack(side=ctk.LEFT, pady = 5)
        
        #終了ボタン
        quit_button = ctk.CTkButton(button_frame, text="×", command=self.master.quit, border_color= "gray", fg_color= "#292929", border_width=2, hover_color= "#414141", width = 100)
        quit_button.pack(side=ctk.RIGHT, padx=5, pady = 5)
        
        
        


        #*****************************アルゴリズムパラメータ設定*************************************
        self.root = ctk.CTkScrollableFrame(self.param_frame, fg_color= "#212121")
        self.root.pack(padx=5, fill = ctk.BOTH, expand = True, pady = 5)
        
        param_label = ctk.CTkLabel(self.root, text = "Algorithm settings", fg_color= "#212121")
        param_label.pack(anchor="w")

        self.algo_setting_frame = ctk.CTkFrame(self.root, fg_color= "#292929")
        self.algo_setting_frame.pack(pady=5, padx=5, fill=ctk.X)
        
        """アルゴリズム選択"""
        algo_frame = ctk.CTkFrame(self.algo_setting_frame, fg_color= "transparent")
        algo_frame.pack(anchor="w", padx=10)
        algo_label = ctk.CTkLabel(algo_frame, text="Algorithm:       ", fg_color= "transparent")
        algo_label.pack(side = ctk.LEFT, pady=5)

        self.algo_abc_var = ctk.BooleanVar(value=True)
        self.algo_abc_button = ctk.CTkCheckBox(algo_frame, text="MOABC", variable=self.algo_abc_var, width=50, checkbox_width = 20,  checkbox_height = 20, onvalue= True, offvalue= False,fg_color = "#4772B3", hover_color = "#5DB6EA")
        self.algo_abc_button.pack(side=ctk.LEFT, padx=5)
        
        self.algo_gwo_var = ctk.BooleanVar(value=False)
        self.algo_gwo_button = ctk.CTkCheckBox(algo_frame, text="MOGWO", variable=self.algo_gwo_var, width=50, checkbox_width = 20,  checkbox_height = 20, onvalue= True, offvalue= False,fg_color = "#4772B3", hover_color = "#5DB6EA")
        self.algo_gwo_button.pack(side=ctk.LEFT, padx=5)
        

        """世代の最大数"""
        max_gen_frame = ctk.CTkFrame(self.algo_setting_frame, fg_color= "transparent")
        max_gen_frame.pack(anchor="w", padx= 5, pady=5)

        max_gen_label = ctk.CTkLabel( max_gen_frame, text="Maximum Generation:", fg_color= "transparent")
        max_gen_label.pack(side=ctk.LEFT, padx=5)
        self.max_gen_var = ctk.StringVar(value="1000")
        self.max_gen_entry = ctk.CTkEntry( max_gen_frame, textvariable=self. max_gen_var, width=50)
        self.max_gen_entry.pack(side=ctk.LEFT, padx=5)
        
        """個体数"""
        population_size_frame = ctk.CTkFrame(self.algo_setting_frame, fg_color= "#292929")
        population_size_frame.pack(padx=5, pady=5,anchor="w")
        population_size_label = ctk.CTkLabel(population_size_frame, text="Population Size:", fg_color= "#292929")
        population_size_label.pack(side=ctk.LEFT, padx=5)
        self.population_size_var = ctk.StringVar(value="100")
        self.population_size_entry = ctk.CTkEntry(population_size_frame, textvariable=self.population_size_var, width=50)
        self.population_size_entry.pack(side=ctk.LEFT, padx=5)
        
        """External archiveの最大容量"""
        EA_count_frame = ctk.CTkFrame(self.algo_setting_frame, fg_color= "#292929")
        EA_count_frame.pack( anchor="w", padx=5, pady=5,)
        EA_count_label = ctk.CTkLabel(EA_count_frame, text="Capacity of External Archive:", fg_color= "#292929")
        EA_count_label.pack(side=ctk.LEFT, padx=5)
        self.EA_count_var = ctk.StringVar(value="100")
        self.EA_count_entry = ctk.CTkEntry(EA_count_frame, textvariable=self.EA_count_var, width=50)
        self.EA_count_entry.pack(side=ctk.LEFT, padx=5)
        

         #*****************************関数設定*************************************
        func_label = ctk.CTkLabel(self.root, text = "\n\nFunction settings", fg_color= "#212121")
        func_label.pack(anchor="w")
        self.func_frame = ctk.CTkFrame(self.root, fg_color= "#292929")
        self.func_frame.pack(pady=10, padx=5, fill=ctk.X)
        
        """補助関数と変数の個数"""
        func_count_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        func_count_frame.pack(pady=5, padx=5, anchor="w")

        variable_count_label = ctk.CTkLabel(func_count_frame, text="Number of variables:", fg_color= "#292929")
        variable_count_label.pack(side=ctk.LEFT, padx=5)
        self.variable_count_var = ctk.StringVar(value="10")
        self.variable_count_entry = ctk.CTkEntry(func_count_frame, textvariable=self.variable_count_var, width=50)
        self.variable_count_entry.pack(side=ctk.LEFT, padx=5)

        subfunc_count_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        subfunc_count_frame.pack(pady=5, padx=5, anchor="w")
        subfunc_count_label = ctk.CTkLabel(subfunc_count_frame, text="Number of sub-functions:", fg_color= "#292929")
        subfunc_count_label.pack(side=ctk.LEFT, padx=5)
        self.subfunc_count_var = ctk.StringVar(value="2")
        self.subfunc_count_entry = ctk.CTkEntry(subfunc_count_frame, textvariable=self.subfunc_count_var, width=50)
        self.subfunc_count_entry.pack(side=ctk.LEFT, padx=5)
        

        """変数の範囲入力"""
        self.variable_min_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        self.variable_min_frame.pack(pady = 5, padx=5, anchor="w")
        self.variable_min_label = ctk.CTkLabel(self.variable_min_frame, text="Minimum values: ", fg_color= "#292929")
        self.variable_min_label.pack(side=ctk.LEFT, padx=5)
        self.variable_min_var= ctk.StringVar(value="0, 0, 0, 0, 0, 0, 0, 0, 0, 0")
        self.variable_min_entry = ctk.CTkEntry(self.variable_min_frame, textvariable=self.variable_min_var, width=300)
        self.variable_min_entry.pack(side=ctk.LEFT, padx=5)
        
        self.variable_max_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        self.variable_max_frame.pack(pady = 5, padx=5, anchor="w")
        self.variable_max_label = ctk.CTkLabel(self.variable_max_frame, text="Maximum values:", fg_color= "#292929")
        self.variable_max_label.pack(side=ctk.LEFT, padx=5)
        self.variable_max_var= ctk.StringVar(value="1, 1, 1, 1, 1, 1, 1, 1, 1, 1")
        self.variable_max_entry = ctk.CTkEntry(self.variable_max_frame, textvariable=self.variable_max_var, width=300)
        self.variable_max_entry.pack(side=ctk.LEFT, padx=5)
        
        
        """補助関数入力"""
        self.subfuncs_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        self.subfuncs_frame.pack(padx=5, anchor = "w")
        self.subfuncs = []
        self.subfuncs_var = []
        self.initial_input = ["1+10*sum(2, N, x[i])/(N-1)", "1-(x[1]/g[1])**0.25-x[1]/g[1]*sin(10*pi*x[1])"]
        self.update_subfunc_entries()
        self.subfunc_count_var.trace_add("write", self.update_subfunc_entries)


        """関数入力"""
        """F1"""
        func1_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        func1_frame.pack(pady=5, padx=5)
        func1_label = ctk.CTkLabel(func1_frame, text="f_1:", fg_color= "transparent")
        func1_label.pack(side=ctk.LEFT, padx=5)
        self.func1_var= ctk.StringVar(value="x[1]")
        self.func1_entry = ctk.CTkEntry(func1_frame, width=400, textvariable=self.func1_var)
        self.func1_entry.pack(side = ctk.BOTTOM, fill=ctk.X, padx=5)
        """F2"""
        func2_frame = ctk.CTkFrame(self.func_frame, fg_color= "transparent")
        func2_frame.pack(pady=5, padx=5)
        func2_label = ctk.CTkLabel(func2_frame, text=f"f_2:", fg_color= "transparent")
        func2_label.pack(side=ctk.LEFT, padx=5)
        self.func2_var= ctk.StringVar(value="g[1]*g[2]")
        self.func2_entry = ctk.CTkEntry(func2_frame, width=400, textvariable=self.func2_var)
        self.func2_entry.pack(side = ctk.BOTTOM, fill=ctk.X, padx=5)
        
    #*****************************最適解設定*************************************
        
        self.pareto_optimal_frame = ctk.CTkFrame(self.root, fg_color= "#292929")
        self.pareto_optimal_frame.pack(pady=10, padx=5, fill=ctk.X, side = ctk.BOTTOM)
        

        self.pareto_optimal_var= ctk.BooleanVar(value=False)
        self.pareto_optimal_entry = ctk.CTkCheckBox(self.pareto_optimal_frame, width=50,checkbox_width = 20,  checkbox_height = 20,text=f"Show true pareto front", variable=self.pareto_optimal_var, onvalue= True, offvalue= False,fg_color = "#4772B3", hover_color = "#5DB6EA")
        self.pareto_optimal_entry.pack(padx=5, anchor = "w")

        """変数の範囲入力"""
        self.pareto_min_frame = ctk.CTkFrame(self.pareto_optimal_frame, fg_color= "transparent")
        self.pareto_min_frame.pack(padx = 5, anchor="w", pady=5)
        self.pareto_min_label = ctk.CTkLabel(self.pareto_min_frame, text=" Minimum value:", fg_color= "transparent")
        self.pareto_min_label.pack(side=ctk.LEFT, padx=5)
        self.pareto_min_var= ctk.StringVar(value="0")
        self.pareto_min_entry = ctk.CTkEntry(self.pareto_min_frame, textvariable=self.pareto_min_var, width=70)
        self.pareto_min_entry.pack(side=ctk.LEFT, padx=5)
        
        self.pareto_max_frame = ctk.CTkFrame(self.pareto_optimal_frame, fg_color= "transparent")
        self.pareto_max_frame.pack(padx = 5, anchor="w", pady= 5)
        self.pareto_max_label = ctk.CTkLabel(self.pareto_max_frame, text="Maximum value:", fg_color= "transparent")
        self.pareto_max_label.pack(side=ctk.LEFT, padx=5)
        self.pareto_max_var= ctk.StringVar(value="1")
        self.pareto_max_entry = ctk.CTkEntry(self.pareto_max_frame, textvariable=self.pareto_max_var, width=70)
        self.pareto_max_entry.pack(side=ctk.LEFT, padx=5)
        
        """関数入力"""
        """F1"""
        pareto_func1_frame = ctk.CTkFrame(self.pareto_optimal_frame, fg_color= "transparent")
        pareto_func1_frame.pack(pady=5, padx=5)
        pareto_func1_label = ctk.CTkLabel(pareto_func1_frame, text=f"f_1:", fg_color= "transparent")
        pareto_func1_label.pack(side=ctk.LEFT, padx=5)
        self.pareto_func1_var= ctk.StringVar(value="x[1]")
        self.pareto_func1_entry = ctk.CTkEntry(pareto_func1_frame, width=400, textvariable=self.pareto_func1_var)
        self.pareto_func1_entry.pack(fill=ctk.X, padx=5)
        """F2"""
        pareto_func2_frame = ctk.CTkFrame(self.pareto_optimal_frame, fg_color= "transparent")
        pareto_func2_frame.pack(pady=5, padx=5)
        pareto_func2_label = ctk.CTkLabel(pareto_func2_frame, text=f"f_2:", fg_color= "transparent")
        pareto_func2_label.pack(side=ctk.LEFT, padx=5)
        self.pareto_func2_var= ctk.StringVar(value="1-x[1]**0.25-x[1]*sin(10*pi*x[1])")
        self.pareto_func2_entry = ctk.CTkEntry(pareto_func2_frame, width=400, textvariable=self.pareto_func2_var)
        self.pareto_func2_entry.pack(fill=ctk.X, padx=5)
        
        
        pareto_label = ctk.CTkLabel(self.root, text = "\n\nTrue pareto front settings")
        pareto_label.pack(anchor="w", side = ctk.BOTTOM)
        
        
        """エラー表示"""
        self.errors_list = []

        
        

  
    """補助関数のテキストボックスを更新"""
    def update_subfunc_entries(self, *args):
        self.subfuncs_var.clear()
        try:
            subfunc_count_str = self.subfunc_count_var.get()
            if subfunc_count_str == "":
                raise ValueError()
            else :
                self.subfunc_count = int(subfunc_count_str)
        except:
            for widget in self.subfuncs_frame.winfo_children():
                widget.destroy()
            self.subfuncs.clear()
            subfunc_frame = ctk.CTkFrame(self.subfuncs_frame, fg_color= "#292929")
            subfunc_frame.pack(pady=0)
            subfunc_label = ctk.CTkLabel(subfunc_frame, text=" ", fg_color= "#292929", height = 0)
            subfunc_label.pack(side=ctk.LEFT, padx=5)
            return
        for widget in self.subfuncs_frame.winfo_children():
            widget.destroy()
        self.subfuncs.clear()
        for i in range(self.subfunc_count):
            subfunc_frame = ctk.CTkFrame(self.subfuncs_frame, fg_color= "#292929")
            subfunc_frame.pack(pady=5)
            subfunc_label = ctk.CTkLabel(subfunc_frame, text=f"g_{i+1}:", fg_color= "#292929")
            subfunc_label.pack(side=ctk.LEFT, padx=5)
            if (i < len(self.initial_input)):
                subfunc_var = ctk.StringVar(value=self.initial_input[i])
            else:
                subfunc_var = ctk.StringVar(value="")
            subfunc_entry = ctk.CTkEntry(subfunc_frame, width=400, textvariable=subfunc_var)
            subfunc_entry.pack(fill=ctk.X, padx=5)
            self.subfuncs_var.append(subfunc_var)
        self.initial_input = []
   
        
            
    """エラーを表示"""
    def display_error(self, master):
        masters = ["Algorithm settings", "Function settings", "True pareto optimal solution settings"]
        error_text = "In "+masters[master]+"\n"
        if (len(self.errors_list) != 0):
            for i in range(len(self.errors_list)):
                error_text+= "Error: "+self.errors_list[i]+"\n"
            error_text.rstrip("\n")
            tk.messagebox.showerror("Error" ,error_text)


            
    """入力エラーチェック"""      
    def error_check(self):
        self.errors_list.clear()
        """check algorithm settings"""
        self.mode_abc = self.algo_abc_var.get()
        self.mode_gwo = self.algo_gwo_var.get()
        if (self.mode_abc == False and self.mode_gwo == False):
            self.errors_list.append("No algorithm selected")
    
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(0)
            return


        try:
            self.population_size = int(self.population_size_var.get())
        except:
            self.errors_list.append("Population Size must be a positive integer value")
        else:
            if self.population_size <= 0:
                self.errors_list.append("Population Size must be a positive integer value")
                
        try:
            self.max_gen = int(self.max_gen_var.get())
        except:
            self.errors_list.append("Maximum Generation must be a positive integer value")
        else:
            if self.max_gen <= 0:
                self.errors_list.append("Maximum Generation max must be a positive integer value")
        
        try:
            self.EA_count = int(self.EA_count_var.get())
        except:
            self.errors_list.append("Capacity of external archive must be a positive integer value")
        else:
            if self.EA_count <= 0:
                self.errors_list.append("Capacity of external archive must be a positive integer value")
             
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(0)
            return 

        """check function settings"""
        try:
            self.variable_count = int(self.variable_count_var.get())
        except:
            self.errors_list.append("Number of variables must be a positive integer value")
        else:
            if self.variable_count <= 0:
                self.errors_list.append("Number of variables must be a positive integer value")
        
            
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(1)
            return
        
        valid_maxmin_value = True
        try:
            self.variable_min = np.array([Formula(x, self.variable_count) for x in self.variable_min_var.get().split(',')])
            self.variable_min = np.array([f.calculate() for f  in self.variable_min])
        except ValueError:
            self.errors_list.append("Minimum values should be real numbers separated by \",\"")
            valid_maxmin_value = False
        else:
            if (len(self.variable_min) != self.variable_count):
                self.errors_list.append("Number of minimum values does not match the number of\n"+ " "*10+"variables")
                valid_maxmin_value = False
        try:
            self.variable_max = np.array([Formula(x, self.variable_count) for x in self.variable_max_var.get().split(',')])
            self.variable_max = np.array([f.calculate() for f  in self.variable_max])
        except ValueError:
            self.errors_list.append("Maximum values should be real numbers separated by\" ,\"")
            valid_maxmin_value = False
        else:
            if (len(self.variable_max) != self.variable_count):
                self.errors_list.append("Number of maximum values does not match the number of\n"+ " "*10+"variables")
                valid_maxmin_value = False
        if (valid_maxmin_value):
            if not (np.all(self.variable_max >= self.variable_min)):
                self.errors_list.append("Maximum values must be greater than min values")
        
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(1)
            return
        
        self.func1 = self.func1_var.get()
        self.func2 = self.func2_var.get()
        self.subfuncs = [self.subfuncs_var[i].get() for i in range(self.subfunc_count)]
        self.func = Function(self.func1, self.func2,copy.deepcopy(self.subfuncs), self.variable_max, self.variable_min)
        self.errors_list = self.func.error_check()
        
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(1)
            return 
        
        
        """check pareto optimal settings"""
        self.pareto_optimal = self.pareto_optimal_var.get()
        if (self.pareto_optimal == True):
            self.pareto_variable_count = 1
            valid_maxmin_value = True
            
            try: 
                self.pareto_min_expr = Formula(self.pareto_min_var.get(), self.pareto_variable_count)
                self.pareto_min = np.array([self.pareto_min_expr.calculate()])
            except ValueError:
                self.errors_list.append("Minimum value must be a real number")
                valid_maxmin_value = False
            try: 
                self.pareto_max_expr = Formula(self.pareto_max_var.get(), self.pareto_variable_count)
                self.pareto_max = np.array([self.pareto_max_expr.calculate()])
            except ValueError:
                self.errors_list.append("Maximum value must be a real number")
                valid_maxmin_value = False

            if (valid_maxmin_value):
                if not (np.all(self.pareto_max >= self.pareto_min)):
                    self.errors_list.append("Maximum value must be greater than minimum values")
                    
            if (len(self.errors_list) != 0):
                self.running = False
                self.display_error(2)
                return 
            
            self.pareto_func1 = self.pareto_func1_var.get()
            self.pareto_func2 = self.pareto_func2_var.get()
            self.pareto_func = Function(self.pareto_func1, self.pareto_func2, [], self.pareto_max, self.pareto_min)
            self.errors_list = self.pareto_func.error_check()
        else:
            self.x_data_theory = np.array([])
            self.y_data_theory = np.array([])
        
        if (len(self.errors_list) != 0):
            self.running = False
            self.display_error(2)
            
    """グラフを更新"""   
    def update_graph(self):
        if self.running:
            if (self.mode_abc == True):    
                self.moabc.update_generation()
                self.x_data_abc = self.moabc.get_EA_f1()
                self.y_data_abc = self.moabc.get_EA_f2() 
                x_min_abc, x_max_abc = np.min(self.x_data_abc), np.max(self.x_data_abc)
                y_min_abc, y_max_abc = np.min(self.y_data_abc), np.max(self.y_data_abc)
                offsets_abc = np.column_stack((self.x_data_abc, self.y_data_abc)) 
                self.scatter_abc.set_offsets(offsets_abc)
            if (self.mode_gwo == True):
                self.mogwo.update_generation()
                self.x_data_gwo = self.mogwo.get_EA_f1()
                self.y_data_gwo = self.mogwo.get_EA_f2()
                x_min_gwo, x_max_gwo = np.min(self.x_data_gwo), np.max(self.x_data_gwo)
                y_min_gwo, y_max_gwo = np.min(self.y_data_gwo), np.max(self.y_data_gwo)
                offsets_gwo = np.column_stack((self.x_data_gwo, self.y_data_gwo))
                self.scatter_gwo.set_offsets(offsets_gwo)
                
            #グラフの範囲を設定
            if (self.mode_abc == True and self.mode_gwo == True):
                x_min = min(x_min_abc, x_min_gwo)
                x_max = max(x_max_abc, x_max_gwo)
                y_min = min(y_min_abc, y_min_gwo)
                y_max = max(y_max_abc, y_max_gwo)
            elif (self.mode_abc == True):
                x_min, x_max = x_min_abc, x_max_abc
                y_min, y_max = y_min_abc, y_max_abc
            elif (self.mode_gwo == True):
                x_min, x_max = x_min_gwo, x_max_gwo
                y_min, y_max = y_min_gwo, y_max_gwo
                
            self.ax.set_xlim(x_min-0.1, x_max+0.1) 
            self.ax.set_ylim(y_min-0.1, y_max+0.1)
            
            generation = 0
            if (self.mode_abc == True):
                generation = self.moabc.generation
            elif(self.mode_gwo == True):
                generation = self.mogwo.generation
            self.ax.set_title(f"Generation : {generation}")
            
            if  generation == self.max_gen:
                self.sim_start = False
                self.running = False
            if (self.sim_start == False):
                self.start_button.configure( fg_color= "#292929", text_color = "white")
                self.stop_button.configure( fg_color= "#414141", text_color="gray")

            self.canvas.draw()    
            
            self.root.after(1, self.update_graph)  # 100ms後に再実行
           
    """シミュレーションを開始"""
    def start_simulation(self):
        if not self.running:
            self.running = True


            self.error_check()
            
            if self.running:
                self.stop_button.configure( fg_color= "#292929", hover_color= "#414141", text_color="white", text = "▮▮")
                self.start_button.configure( fg_color= "#414141", text_color="gray")
                self.sim_start = True
                if (self.pareto_optimal == True):
                    #理論曲線を描画
                    f_theory = []
                    x_theory = []
                    count = np.sum((self.pareto_max-self.pareto_min) != 0)
                    for i in range(self.pareto_variable_count):
                        if (self.pareto_max[i] == self.pareto_min[i]):
                            x_theory.append(np.array([self.pareto_max[i]]))
                        else:
                            x_theory.append(np.linspace(self.pareto_min[i], self.pareto_max[i], int(np.round(1000**(1/count)))))
                    x_theory = np.array(list(product(*x_theory)))

                    for i in range(len(x_theory)):
                        f1, f2 = self.pareto_func.calculate_func(x_theory[i])
                        f_theory.append([f1, f2])
                    f_theory = np.array(f_theory)
                    f_theory = f_theory[np.argsort(f_theory[:, 0])]
                    self.x_data_theory = f_theory[:, 0]
                    self.y_data_theory = f_theory[:, 1]
                    self.plot.set_xdata(self.x_data_theory)
                    self.plot.set_ydata(self.y_data_theory)
                else:
                    self.plot.set_xdata([])
                    self.plot.set_ydata([])
                
                x_min_gwo, x_max_gwo = 0, 0
                y_min_gwo, y_max_gwo = 0, 0
                x_min_abc, x_max_abc = 0, 0
                y_min_abc, y_max_abc = 0, 0
                      
                #アルゴリズムの初期化      
                if (self.mode_abc == True):    
                    self.moabc = MOABC(self.max_gen, self.population_size, self.EA_count, self.variable_count, self.variable_max, self.variable_min, self.func)
                    self.x_data_abc = self.moabc.get_EA_f1()
                    self.y_data_abc = self.moabc.get_EA_f2()
                    
                    x_min_abc, x_max_abc = np.min(self.x_data_abc), np.max(self.x_data_abc)
                    y_min_abc, y_max_abc = np.min(self.y_data_abc), np.max(self.y_data_abc)
                else:
                    self.x_data_abc = np.array([])
                    self.y_data_abc = np.array([])
                if (self.mode_gwo == True):
                    self.mogwo = MOGWO(self.max_gen, self.population_size, self.EA_count, self.variable_count, self.variable_max, self.variable_min, self.func)
                    self.x_data_gwo = self.mogwo.get_EA_f1()
                    self.y_data_gwo = self.mogwo.get_EA_f2()
                    x_min_gwo, x_max_gwo = np.min(self.x_data_gwo), np.max(self.x_data_gwo)
                    y_min_gwo, y_max_gwo = np.min(self.y_data_gwo), np.max(self.y_data_gwo)
                else:
                    self.x_data_gwo = np.array([])
                    self.y_data_gwo = np.array([])
                
                offsets_abc = np.column_stack((self.x_data_abc, self.y_data_abc)) 
                self.scatter_abc.set_offsets(offsets_abc)
                offsets_gwo = np.column_stack((self.x_data_gwo, self.y_data_gwo))
                self.scatter_gwo.set_offsets(offsets_gwo)
                #グラフの範囲を設定
                if (self.mode_abc == True and self.mode_gwo == True):
                    x_min = min(x_min_abc, x_min_gwo)
                    x_max = max(x_max_abc, x_max_gwo)
                    y_min = min(y_min_abc, y_min_gwo)
                    y_max = max(y_max_abc, y_max_gwo)
                    self.ax.legend(handles=[self.scatter_abc, self.scatter_gwo], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=2,frameon=False )
                elif (self.mode_abc == True):
                    x_min, x_max = x_min_abc, x_max_abc
                    y_min, y_max = y_min_abc, y_max_abc
                    self.ax.legend(handles=[self.scatter_abc], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=1,frameon=False )
                elif (self.mode_gwo == True):
                    x_min, x_max = x_min_gwo, x_max_gwo
                    y_min, y_max = y_min_gwo, y_max_gwo
                    self.ax.legend(handles=[self.scatter_gwo], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=1,frameon=False )
                    
                self.ax.set_xlim(x_min-0.1, x_max+0.1) 
                self.ax.set_ylim(y_min-0.1, y_max+0.1)
                
                self.canvas.draw()
                self.update_graph()
    
    """シミュレーションを停止"""
    def stop_simulation(self):
        if self.sim_start == True:
            self.running = not self.running
            if self.running:
                self.update_graph()
                self.stop_button.configure( fg_color= "#292929", hover_color= "#414141", text = "▮▮")
                self.start_button.configure( fg_color= "#414141", text_color = "grey")
            else:
                self.stop_button.configure(fg_color = "#292929", hover_color = "#414141", text = "▶")
                self.start_button.configure( fg_color= "#292929", text_color = "white")
            generation = 0
            if (self.mode_abc == True):
                generation = self.moabc.generation
            if(self.mode_gwo == True):
                generation = self.mogwo.generation
            title = f"Generation : {generation}"

            self.ax.set_title(title)
            self.canvas.draw()

    """被覆率を計算"""
    def coverage_rate(self, f1, f2):
        y_t = np.array([])
        if (len(f1) == 0):
            return " "
        
        covered_count = 0
        n = len(f1)
        if self.pareto_optimal == False:
            y_t = np.linspace(np.min(f2), np.max(f2), n)
        else:
            x_t = np.linspace(self.pareto_min[0], self.pareto_max[0], n)
            f1_theory = np.zeros(n)
            f2_theory = np.zeros(n)
            
            for i in range(n):
                f1_theory[i], f2_theory[i] = self.pareto_func.calculate_func([x_t[i]])
            y_t = np.linspace(np.min(f2_theory), np.max(f2_theory), n)
        for i in range(n-1):
            f2_min = min(y_t[i], y_t[i+1])
            f2_max = max(y_t[i], y_t[i+1])
            for j in range(n):
                if f2_min <= f2[j] <=f2_max:
                    covered_count +=1
                    break
        coverage_rate = covered_count / n
        return f"{coverage_rate:.3f}"
    
    """優越個体割合を計算"""
    def RNI(self, x_data1, y_data1, x_data2, y_data2):
        n1 = len(x_data1)
        n2 = len(x_data2)
        count1 = 0.0
        count2 = 0.0
        for i in range(n1):
            is_nondominant = True
            for j in range(n2):
                if x_data1[i] > x_data2[j] and y_data1[i] > y_data2[j]:
                    is_nondominant = False
                    break
            if is_nondominant:
                count1 += 1.0
        for i in range(n2):
            is_nondominant = True
            for j in range(n1):
                if x_data2[i] > x_data1[j] and y_data2[i] > y_data1[j]:
                    is_nondominant = False
                    break
            if is_nondominant:
                count2 += 1.0
        count1 = count1/n1
        count2 = count2/n2
        RNI = count1/(count1+count2)
        return f"{RNI:.3f}"
        
        
    
    def show_CR(self):
        message = ""
        if self.mode_abc:
            CR_abc = self.coverage_rate(self.x_data_abc, self.y_data_abc)
            message += f"MOABC:\n  CR = {CR_abc}\n"
        if self.mode_gwo:
            CR_gwo = self.coverage_rate(self.x_data_gwo, self.y_data_gwo)
            message += f"MOGWO:\n  CR = {CR_gwo}\n"
        if self.mode_abc or self.mode_gwo:
            tk.messagebox.showinfo("Show CR" ,message)
        
            
        
    
    def show_RNI(self):
        if not (self.mode_abc and self.mode_gwo):
            tk.messagebox.showinfo("RNI" ,f"Please check both algorithms to calculate RNI")
        rni_abc = self.RNI(self.x_data_abc, self.y_data_abc, self.x_data_gwo, self.y_data_gwo)
        rni_gwo = self.RNI(self.x_data_gwo, self.y_data_gwo, self.x_data_abc, self.y_data_abc)
        tk.messagebox.showinfo("Show RNI" ,f"MOABC:\n  RNI = {rni_abc}\n"+f"MOGWO:\n  RNI = {rni_gwo}")
        
        
    def save_function(self):
        file_path = tk.filedialog.asksaveasfilename(
            title="Save function",
            defaultextension=".dat",
            initialfile = "output.dat",
            filetypes=[("dat file", "*.dat"), ("All file", "*.*")]
        )
        if file_path:
            with open(file_path, "wb") as fp:
                
                variable_count_str = self.variable_count_var.get()
                variable_min_var_str = self.variable_min_var.get()
                variable_max_var_str = self.variable_max_var.get()
                subfunc_count_str = self.subfunc_count_var.get()
                subfuncs = [self.subfuncs_var[i].get() for i in range(int(subfunc_count_str))]
                func1 = self.func1_var.get()
                func2 = self.func2_var.get()
                pareto_min_var_str = self.pareto_min_var.get()
                pareto_max_var_str = self.pareto_max_var.get()
                pareto_func1 = self.pareto_func1_var.get()
                pareto_func2 = self.pareto_func2_var.get()
                try:
                    fp.write("***start***".encode("ascii")+ b"\n")
                    fp.write(variable_count_str.encode("ascii") + b"\n")
                    fp.write(variable_min_var_str.encode("ascii") + b"\n")
                    fp.write(variable_max_var_str.encode("ascii") + b"\n")
                    fp.write(subfunc_count_str.encode("ascii") + b"\n")
                    for subfunc in subfuncs:
                        fp.write(subfunc.encode("ascii") + b"\n")
                    
                    fp.write(func1.encode("ascii") + b"\n")
                    fp.write(func2.encode("ascii") + b"\n")
                    fp.write(pareto_min_var_str.encode("ascii") + b"\n")
                    fp.write(pareto_max_var_str.encode("ascii") + b"\n")
                    fp.write(pareto_func1.encode("ascii") + b"\n")
                    fp.write(pareto_func2.encode("ascii") + b"\n")
                    fp.write("***end***".encode("ascii")+ b"\n")
                except UnicodeEncodeError as e:
                    tk.messagebox.showerror("Error", f"Invalid text used: {e}")
                else:
                    tk.messagebox.showinfo("Info" ,f"Function setting saved successfully")
        else:
            tk.messagebox.showinfo("Info" ,f"Saving function setting cancelled")
    
    class Invalid_Function_Error(Exception):
        def __init__(self, file_path):
            self.file_path = file_path
            self.message = f"Invalid function file {file_path}"
            super().__init__(self.message)
        
    def load_function(self):
        file_path = tk.filedialog.askopenfilename(
            title="Load function",
            filetypes=[("dat file", "*.dat"), ("All file", "*.*")]
        )
        if file_path:
            try:
                with open(file_path, "rb") as fp:
                    lines = fp.readlines()
                    if (lines[0].decode("ascii").strip() !="***start***" or lines[-1].decode("ascii").strip() != "***end***"):
                        raise self.Invalid_Function_Error(file_path)
                    variable_count_str = lines[1].decode("ascii").strip()
                    variable_min_var_str = lines[2].decode("ascii").strip()
                    variable_max_var_str = lines[3].decode("ascii").strip()
                    subfunc_count_str = lines[4].decode("ascii").strip()
                    self.subfunc_count = int(subfunc_count_str)
                    
                    subfuncs = [lines[i].decode("ascii").strip() for i in range(5, 5 + int(self.subfunc_count))]
                    
                    func1 = lines[5 + int(subfunc_count_str)].decode("ascii").strip()
                    func2 = lines[6 + int(subfunc_count_str)].decode("ascii").strip()
                    pareto_min_var_str = lines[7 + int(subfunc_count_str)].decode("ascii").strip()
                    pareto_max_var_str = lines[8 + int(subfunc_count_str)].decode("ascii").strip()
                    pareto_func1 = lines[9 + int(subfunc_count_str)].decode("ascii").strip()
                    pareto_func2 = lines[10 + int(subfunc_count_str)].decode("ascii").strip()
                    
                    self.variable_count_var.set(variable_count_str)
                    self.variable_min_var.set(variable_min_var_str)
                    self.variable_max_var.set(variable_max_var_str)
                    self.subfunc_count_var.set(subfunc_count_str)
                    
                    self.initial_input.clear()
                    for subfunc in subfuncs:
                        self.initial_input.append(subfunc)
                    
                    self.update_subfunc_entries()
                    self.func1_var.set(func1)
                    self.func2_var.set(func2)
                    self.pareto_min_var.set(pareto_min_var_str)
                    self.pareto_max_var.set(pareto_max_var_str)
                    self.pareto_func1_var.set(pareto_func1)
                    self.pareto_func2_var.set(pareto_func2)
                    
            except UnicodeDecodeError as e:
                tk.messagebox.showerror("Error", f"Decodig Error: {e}")
            except Exception as e:
                tk.messagebox.showerror("Error", f"Error: {e}")
            except self.Invalid_Function_Error as e:
                tk.messagebox.showerror("Error", f"{e}")
        else:
            tk.messagebox.showinfo("Info" ,f"Loading function setting cancelled")
        
    
    def save_figure(self):
        fig1 = plt.Figure(figsize=(7, 5), dpi = 140) 
        plt.rcParams.update({'text.color': 'k', 
                'axes.labelcolor': 'k', 
                'xtick.color': 'k', 
                'ytick.color': 'k', 
                'axes.titlecolor': 'k'})
        ax = fig1.add_subplot(111)
        ax.set_xlabel(r"$f_1$", labelpad=10, fontsize = 14)
        ax.set_ylabel(r"$f_2$", labelpad=10, fontsize = 14)
        ax.xaxis.set_label_coords(0.5, -0.1)
        ax.yaxis.set_label_coords(-0.1, 0.5)
        ax.grid(linestyle='--', linewidth=0.5)
                
        ax.plot(self.x_data_theory, self.y_data_theory, c = 'r', ls = '--', lw = 1.5, zorder=1) 
        scatter_abc = ax.scatter(self.x_data_abc, self.y_data_abc, c='royalblue', s=40, zorder=2, edgecolor = "whitesmoke", linewidth = 1, label = "MOABC")
        scatter_gwo = ax.scatter(self.x_data_gwo, self.y_data_gwo, marker = 'd', c='lawngreen', s=40, zorder=2, edgecolor = "whitesmoke", linewidth = 1, label = "MOGWO")
        
        if (self.mode_abc == True and self.mode_gwo == True):
            ax.legend(handles=[scatter_abc, scatter_gwo], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=2,frameon=False )
        elif (self.mode_abc == True):
            ax.legend(handles=[scatter_abc], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=1,frameon=False )
        elif (self.mode_gwo == True):
            ax.legend(handles=[scatter_gwo], loc="lower center", bbox_to_anchor=(0.8, -0.15), borderaxespad=0, ncol=1,frameon=False )
            
        
        plt.rcParams.update({'text.color': 'white', 
                'axes.labelcolor': 'white', 
                'xtick.color': 'white', 
                'ytick.color': 'white', 
                'axes.titlecolor': 'white'})
        
        file_path = tk.filedialog.asksaveasfilename(
            title="Save Figure",
            defaultextension=".pdf",
            initialfile = "output.pdf",
            filetypes=[("PNG file", "*.png"), ("PDF file", "*.pdf"), ("All file", "*.*")]
        )
        if file_path:
            fig1.savefig(file_path, dpi=300, bbox_inches="tight")
            tk.messagebox.showinfo("Info" ,f"Figure saved successfully")
        else:
            tk.messagebox.showinfo("Info" ,f"Saving figure cancelled")

class Function:
    def __init__(self, f1_expr, f2_expr, subfuncs_expr, x_max, x_min):
        
        self.x_max = x_max
        self.x_min = x_min
        self.x_size = len(x_min)
        self.subfuncs_expr = subfuncs_expr
        self.subfuncs_size = len(subfuncs_expr)
        for i in range(len(subfuncs_expr)):
            self.subfuncs_expr[i] = self.preprocess_expression(subfuncs_expr[i])
        self.f1_expr = self.preprocess_expression(f1_expr)
        self.f2_expr = self.preprocess_expression(f2_expr)
        self.f1 = 0
        self.f2 = 0
        self.subfuncs = np.zeros(len(subfuncs_expr))

    """func_exprとxについて値を計算"""
    def calculate(self, func_expr, x):
        return eval(func_expr, {"__builtins__": None}, {"x": x, "np": np, "g" : self.subfuncs, "SUM" : self.SUM, "IF" : self.IF, "f1": self.f1, "PRODUCT": self.PRODUCT})
    
    """xについてf1, f2の値を計算"""
    def calculate_func(self, x):
        for i in range(len(self.subfuncs_expr)):
            self.subfuncs[i] = self.calculate(self.subfuncs_expr[i], x)
        self.f1 =  self.calculate(self.f1_expr, x)
        self.f2 =  self.calculate(self.f2_expr, x)
        return self.f1, self.f2
     
    """SUM関数"""
    def SUM(self, x, start, end, func_expr):
        result = 0
        if (start < 1):
            raise ValueError()
        if (end < 1):
            raise ValueError()
        if (start > end):
            raise ValueError()
        for i in range(start, end + 1):
            expr = func_expr.replace("x[i]", f"x[{i-1}]")
            expr = expr.replace("x[i+1]", f"x[{i}]")
            expr = expr.replace("x[i-1]", f"x[{i-2}]")
            expr = re.sub(r"\bi\b", f"{i}", expr)

            result += eval(expr, {"__builtins__": None}, {"x": x, "np": np, "IF" : self.IF})
        return result
    
    """PRODUCT関数"""
    def PRODUCT(self, x, start, end, func_expr):
        result = 1
        if (start < 1):
            raise ValueError()
        if (end < 1):
            raise ValueError()
        if (start > end):
            raise ValueError()
        for i in range(start, end + 1):
            expr = func_expr.replace("x[i]", f"x[{i-1}]")
            expr = expr.replace("x[i+1]", f"x[{i}]")
            expr = expr.replace("x[i-1]", f"x[{i-2}]")
            expr = re.sub(r"\bi\b", f"{i}", expr)
            result *= eval(expr, {"__builtins__": None}, {"x": x, "np": np, "IF" : self.IF})
        return result
    
    """IF関数"""
    def IF(self, x, condition, func1, func2):
        if (self.calculate(condition, x)):
            return self.calculate(func1, x)
        return self.calculate(func2, x)

    """前処理"""
    def preprocess_expression(self, expression):
        math_functions = ["sin", "cos", "tan", "sqrt", "log", "exp", "pi", "power",
                          "abs", "arcsin", "arccos", "arctan", "arcsinh",
                          "arccosh", "arctanh", "round", "ceil", "floor", "e", "log10",
                          "log2","sign", "sinh", "cosh", "tanh"] 
        for func in math_functions:
            expression = re.sub(rf'\b{func}\b', f'np.{func}', expression)
        expression = re.sub('N', f'{self.x_size}', expression)
        expression = re.sub('sum', 'SUM', expression)
        expression = re.sub('if', 'IF', expression)
        expression = re.sub('product', 'PRODUCT', expression)
        expression = re.sub(r'f\[1\]', 'f1', expression)
        matches = list(re.finditer(r'SUM\(\s*([^,]+)\s*,\s*([^,]+)\s*,', expression))
        for match in matches:
            tmp = match.end()
            tmp_count = 0
            expression = expression[:tmp+1] + "\"" +expression[tmp+1:]
            while (tmp_count != 1 and tmp < len(expression)):
                if expression[tmp] == '(':
                    tmp_count -= 1
                elif expression[tmp] == ')':
                    tmp_count += 1
                tmp += 1
            expression = expression[:tmp-1] + "\"" +expression[tmp-1:]
        expression = re.sub(r'SUM\(', 'SUM(x, ', expression)
        
        matches = list(re.finditer(r'PRODUCT\(\s*([^,]+)\s*,\s*([^,]+)\s*,', expression))
        for match in matches:
            tmp = match.end()
            tmp_count = 0
            expression = expression[:tmp+1] + "\"" +expression[tmp+1:]
            while (tmp_count != 1 and tmp < len(expression)):
                if expression[tmp] == '(':
                    tmp_count -= 1
                elif expression[tmp] == ')':
                    tmp_count += 1
                tmp += 1
            expression = expression[:tmp-1] + "\"" +expression[tmp-1:]
        expression = re.sub(r'PRODUCT\(', 'PRODUCT(x, ', expression)
            
        matches = list(re.finditer(r'IF\(\s*([^,]+)\s*,\s*([^,]+)\s*,', expression))
        for match in matches:
            tmp = match.end()
            tmp_count = 0
            expression = expression[:tmp+1] + "\"" +expression[tmp+1:]
            while (tmp_count != 1 and tmp < len(expression)):
                if expression[tmp] == '(':
                    tmp_count -= 1
                elif expression[tmp] == ')':
                    tmp_count += 1
                tmp += 1
            expression = expression[:tmp-1] + "\"" +expression[tmp-1:]
        expression = re.sub(r'IF\(\s*([^,]+)\s*,\s*([^,]+)\s*,', self.add_quate, expression)
        expression = re.sub(r'([gx])\[([-\d]+)\]', self.shift_index, expression)
        return expression

    def add_quate(self, match):
        condition, func1 = match.groups()
        return f"IF(x, \"{condition}\", \"{func1}\","

    def shift_index(self, match):
        index = int(match.group(2))  
        return f"{match.group(1)}[{index - 1}]"

    """エラーチェック"""
    def error_check(self):
        errors = []
        x = (self.x_min+self.x_max)/2
        for i in range(len(self.subfuncs_expr)):
            for match in re.finditer(r'g\[(\d+)\]', self.subfuncs_expr[i]):
                if int(match.group(1)) >= i:
                    errors.append(f"g_{i+1} cannot refer g_{int(match.group(1))+1}")
            for match in re.finditer(r'x\[([-\d]+)\]', self.subfuncs_expr[i]):
                if int(match.group(1)) < 0 or int(match.group(1)) >= self.x_size:
                    errors.append(f"x_{int(match.group(1))+1} in g_{i+1} : index out of range")
            for match in re.finditer(r'g\[([-\d]+)\]', self.subfuncs_expr[i]):
                if int(match.group(1)) < 0 or int(match.group(1)) >= self.subfuncs_size:
                    errors.append(f"g_{int(match.group(1))+1} in g_{i+1} : index out of range")
            if re.search(r'f1', self.subfuncs_expr[i]):
                errors.append(f"g_{i+1} cannot refer f_1")
            if re.search(r'f\[2\]', self.subfuncs_expr[i]):
                errors.append(f"g_{i+1} cannot refer f_2")
        if len(errors) > 0:
            return errors
        for i in range(len(self.subfuncs_expr)):
            try:
                self.subfuncs[i] = self.calculate(self.subfuncs_expr[i], x)

            except:
                errors.append(f"g_{i+1}")
                return errors

        for match in re.finditer(r'x\[([-\d]+)\]', self.f1_expr):
            if int(match.group(1)) < 0 or int(match.group(1)) >= self.x_size:
                errors.append(f"x_{int(match.group(1))+1} in f_1 :index out of range") 
        for match in re.finditer(r'g\[([-\d]+)\]', self.f1_expr):
            if int(match.group(1)) < 0 or int(match.group(1)) >= self.subfuncs_size:
                errors.append(f"g_{int(match.group(1))+1} in f_1 : index out of range")
        if re.search(r'f1', self.f1_expr):
            errors.append(f"f_1 cannot refer f_1")
        if re.search(r'f\[2\]', self.f1_expr):
            errors.append(f"f_1 cannot refer f_2")
        if len(errors) > 0:
            return errors
        try:
            self.f1 =  self.calculate(self.f1_expr, x)
        except:
            errors.append(f"f_1")
            
        for match in re.finditer(r'x\[([-\d]+)\]', self.f2_expr):
            if int(match.group(1)) < 0 or int(match.group(1)) >= self.x_size:
                errors.append(f"x_{int(match.group(1))+1} in f_2 : index out of range") 
        for match in re.finditer(r'g\[([-\d]+)\]', self.f2_expr):
            if int(match.group(1)) < 0 or int(match.group(1)) >= self.subfuncs_size:
                errors.append(f"g_{int(match.group(1))+1} in f_2 : index out of range")
        if re.search(r'f\[2\]', self.f1_expr):
            errors.append(f"f_2 cannot refer f_2")
        if len(errors) > 0:
            return errors
        try:
            self.f2 =  self.calculate(self.f2_expr, x)
        except:
            errors.append(f"f_2")
        return errors

class Formula:
    def __init__(self, expression, x_size):
        self.x_size = x_size
        self.expression = self.preprocess_expression(expression)
    
    def preprocess_expression(self, expression):
        math_functions = ["sin", "cos", "tan", "sqrt", "log", "exp", "pi", "power", "abs"] 
        for func in math_functions:
            expression = re.sub(f'{func}', f'np.{func}', expression)
        expression = re.sub('N', f'{self.x_size}', expression)
        return expression
    
    def calculate(self):
        try: 
            result = eval(self.expression, {"__builtins__": None}, {"np": np})
        except:
            raise ValueError()
        else:
            try: 
                result+1
            except:
                raise ValueError
            else:
             return result
      
"""多目的ABCのクラス"""
class MOABC:
    def __init__(self, max_generation, onlooker_size, max_EA_size, x_size, x_max, x_min, func : Function):
        self.max_generation = max_generation    #世代の最大数
        self.generation = 1                     #世代
        self.onlooker_size = onlooker_size      #onlooker bee の数   
        self.max_EA_size = max_EA_size          #外部アーカイブ(external archive)最大容量
        self.x_size = x_size                    #入力ベクトルxの大きさ
        self.x_max = x_max                      #入力ベクトルの各要素の最大値
        self.x_min = x_min                      #入力ベクトルの各要素の最小値
        self.func = func                        #目的関数のFunctionクラス
        self.EA_size = 0                        #外部アーカイブの現在の大きさ
        self.Pm = 0.4
        self.m = int(np.ceil(self.Pm*self.x_size))
        
        
        #外部アーカイブ
        #0:f1,  1:f2, 2-x_size+1: onlooker beeの位置ベクトル
        self.EA = np.zeros((onlooker_size+max_EA_size, x_size+2))

        #現在の各onlooker beeの位置
        #各要素は，0:f1,  1:f2, 2-x_size+1: onlooker beeの位置ベクトル
        self.population = np.zeros((onlooker_size, x_size+2))
        
        #populationをランダムに初期化
        for i in range(onlooker_size):
            for j in range(x_size):
                self.population[i][j+2] = np.random.uniform(x_min[j], x_max[j])
            self.evaluate_fitness(self.population[i])

        #populationをf1の値でソート
        self.population = self.population[np.argsort(self.population[:, 0])]

        #populationの非優越解を外部アーカイブに保存
        for i in range(onlooker_size):
            dominated = False
            f2_i = self.population[i][1]
            for j in range(i):
                if (self.population[j][1] < f2_i):
                    dominated = True
            if not dominated:
                self.EA[self.EA_size] =  self.population[i]
                self.EA_size += 1

    """次世代を計算 """
    def update_generation(self):
        for k in range(self.onlooker_size):
            #ランダムに外部アーカイブから2つ要素を選ぶ
            idx1 = np.random.randint(0, self.EA_size)
            idx2 = np.random.randint(0, self.EA_size)
            while (idx1 == idx2 and self.EA_size >= 2):
                idx2 = np.random.randint(0, self.EA_size)
            EA_x1 = self.EA[idx1]
            EA_x2 = self.EA[idx2]
            
            x = self.population[k]
            v = np.zeros(self.x_size+2)
            #式に従ってx[i]からv[i]を計算
            m_idx_list = np.random.choice(np.arange(0, self.x_size), self.m, replace = False)
            
            for i in range(2, self.x_size+2):
                #ランダムにEA1とEA2を選択してv[i]を計算
                if (i-2 in m_idx_list):
                    v[i] = x[i] + np.random.uniform(0, 2)*(EA_x1[i]-x[i])
                else:
                    v[i] = x[i] + np.random.uniform(0, 2)*(EA_x2[i]-x[i])

                if (v[i] < self.x_min[i-2]):
                    v[i] = self.x_min[i-2]
                elif (v[i] > self.x_max[i-2]):
                    v[i] = self.x_max[i-2]
            #得られたv[i]の評価値を計算
            self.evaluate_fitness(v) 
            
            #onlooker beeの位置を更新
            domination = self.evaluate_domination(x, v)
            if (domination == 1):
                self.EA[self.EA_size] = v
                self.EA_size += 1
                self.population[k] = v
            elif (domination == 0):
                self.EA[self.EA_size] = v
                self.EA_size += 1
                if (np.random.randint(0, 2) == 0):
                    self.population[k] = v
        
        #外部アーカイブを更新
        EA_next = np.zeros((self.onlooker_size+self.max_EA_size, self.x_size+2))
        EA_next_size =0
        self.EA[:self.EA_size] = self.EA[:self.EA_size][np.argsort(self.EA[:self.EA_size, 0])]
        for i in range(self.EA_size):
            dominated = False
            f2_i = self.EA[i][1]
            for j in range(i):
                if (self.EA[j][1] < f2_i):
                    dominated = True
            if not dominated:
                EA_next[EA_next_size] = self.EA[i]
                EA_next_size += 1
        self.EA = EA_next
        self.EA_size = EA_next_size
        
        #外部アーカイブの大きさが容量を超えた場合
        if (self.EA_size > self.max_EA_size):
            EA_crowd_distance = np.zeros((self.EA_size, self.x_size+5))
            EA_crowd_distance[:, 3:] = self.EA[:self.EA_size]
            
            # f1の最近傍距離を計算
            EA_f1_size = np.abs(np.max(self.EA[:self.EA_size, 0])-np.min(self.EA[:self.EA_size, 0]))
            EA_f1_size = max(EA_f1_size, 1e-8)
            #端の値は無限大
            EA_crowd_distance[0][1] = 1e8
            EA_crowd_distance[self.EA_size-1][1] = 1e8

            for i in range(1, self.EA_size-1):
                EA_crowd_distance[i][1] = np.abs(self.EA[i-1][0]-self.EA[i+1][0])/EA_f1_size
                
            # f2の最近傍距離を計算
            EA_crowd_distance = EA_crowd_distance[np.argsort(EA_crowd_distance[:self.EA_size, 4])]
            EA_f2_size =np.abs(np.max(self.EA[:self.EA_size, 1])-np.min(self.EA[:self.EA_size, 1]))
            EA_f2_size = max(EA_f2_size, 1e-8)
            #端の値は無限大
            EA_crowd_distance[0][2] = 1e8
            EA_crowd_distance[self.EA_size-1][2] = 1e8
            
            for i in range(1, self.EA_size-1):
                EA_crowd_distance[i][2] = np.abs(EA_crowd_distance[i-1][4]-EA_crowd_distance[i+1][4])/EA_f2_size
            #crowd distanceを計算
            EA_crowd_distance[:, 0] = EA_crowd_distance[:, 1] + EA_crowd_distance[:, 2]
            
            #crowd distanceでソートして上位max_EA_size個を残す
            EA_crowd_distance = EA_crowd_distance[np.argsort(-EA_crowd_distance[:self.EA_size, 0])]
            self.EA[:self.max_EA_size] = EA_crowd_distance[:self.max_EA_size, 3:]
            self.EA_size = self.max_EA_size
        self.generation+=1
              
    """評価値を計算""" 
    def evaluate_fitness(self, x):
        x[0], x[1] = self.func.calculate_func(x[2:])

    """優越性を評価"""
    def evaluate_domination(self, x1, x2):
        #x1がx2を優越
        if x1[0] <= x2[0] and x1[1] <= x2[1]:
            return -1 
        #x2がx1を優越
        if x1[0] >= x2[0] and x1[1] >= x2[1]:
            return 1
        #互いに優越されない
        return 0
    
    """EAのfunc1の値を取得"""
    def get_EA_f1(self):
        return self.EA[:self.EA_size,0]
    
    """EAのfunc2の値を取得"""
    def get_EA_f2(self):
        return self.EA[:self.EA_size,1]
    
    """集団のfunc1の値を取得"""
    def get_population_f1(self):
        return self.population[:,0]
    
    """集団のfunc2の値を取得"""
    def get_population_f2(self):
        return self.population[:,1]
    
      
"""多目的GWOのクラス"""
class MOGWO:
    def __init__(self, max_generation, wolf_size, max_EA_size, x_size, x_max, x_min, func : Function):
        self.max_generation = max_generation    #世代の最大数
        self.generation = 1                     #世代
        self.wolf_size = wolf_size              #オオカミの数   
        self.max_EA_size = max_EA_size          #外部アーカイブ(external archive)最大容量
        self.x_size = x_size                    #入力ベクトルxの大きさ
        self.x_max = x_max                      #入力ベクトルの各要素の最大値
        self.x_min = x_min                      #入力ベクトルの各要素の最小値
        self.func = func                        #目的関数のFunctionクラス
        self.EA_size = 0 

        #外部アーカイブ
        #各要素は，0:crowd distance,  3:f1,  4:f2, 5-:x_size+5: オオカミの位置ベクトル
        self.EA = np.zeros((wolf_size+max_EA_size, x_size+5))
        
        #現在の各オオカミの位置
        #各要素は，0:f1,  1:f2, 2-x_size+1: オオカミの位置ベクトル
        self.population = np.zeros((wolf_size, x_size+2))
        
        for i in range(wolf_size):
            for j in range(x_size):
                self.population[i][j+2] = np.random.uniform(x_min[j], x_max[j])
            self.evaluate_fitness(self.population[i])

        self.population = self.population[np.argsort(self.population[:, 0])]
    
        for i in range(wolf_size):
            dominated = False
            f2_i = self.population[i][1]
            for j in range(i):
                if (self.population[j][1] < f2_i):
                    dominated = True
            if not dominated:
                self.EA[self.EA_size, 3:] =  self.population[i]
                self.EA_size += 1
        
        #外部アーカイブのcrowd distanceを計算
        self.EA[0][1] = 1e8
        self.EA[self.EA_size-1][1] = 1e8
        if self.EA_size > 3:
            EA_f1_size = np.abs(np.max(self.EA[:self.EA_size, 3])-np.min(self.EA[:self.EA_size, 3]))
            for i in range(1, self.EA_size-1):
                self.EA[i][1] = np.abs(self.EA[i-1][3]-self.EA[i+1][3])/EA_f1_size
                
        self.EA[:self.EA_size] = self.EA[:self.EA_size][np.argsort(self.EA[:self.EA_size, 4])]  
        self.EA[0][2] = 1e8
        self.EA[self.EA_size-1][2] = 1e8
        if self.EA_size > 3:
            EA_f2_size = np.abs(np.max(self.EA[:self.EA_size, 4])-np.min(self.EA[:self.EA_size, 4]))
            for i in range(1, self.EA_size-1):
                self.EA[i][2] = np.abs(self.EA[i-1][4]-self.EA[i+1][4])/EA_f2_size
        
        self.EA[:, 0] = self.EA[:, 1] + self.EA[:, 2]

        
         
    """次世代を計算 """
    def update_generation(self):
        for k in range(self.wolf_size):
            possibilities = self.EA[:self.EA_size, 0]/np.sum(self.EA[:self.EA_size, 0])
            idx1, idx2, idx3 = np.random.choice(self.EA_size, 3,  p = possibilities)
            x_alpha = self.EA[idx1][5:]
            x_beta = self.EA[idx2][5:]
            x_delta = self.EA[idx3][5:]
            x = self.population[k]
            v = np.zeros(self.x_size+2)
            
            a =  np.full(self.x_size, 2.0*(1.0-float(self.generation)/float(self.max_generation)))
            A1 = a*np.random.uniform(-1, 1, self.x_size)
            A2 = a*np.random.uniform(-1, 1, self.x_size)
            A3 = a*np.random.uniform(-1, 1, self.x_size)
            
            C1 = np.random.uniform(0, 2, self.x_size)
            C2 = np.random.uniform(0, 2, self.x_size)
            C3 = np.random.uniform(0, 2, self.x_size)
            D_alpha = np.abs(C1*x_alpha-x[2:])
            D_beta = np.abs(C2*x_beta-x[2:])
            D_delta = np.abs(C3*x_delta-x[2:])
            x1 = x_alpha-A1*D_alpha
            x2 = x_beta-A2*D_beta
            x3 = x_delta-A3*D_delta
            
            v[2:] = (x1+x2+x3)/3

            for i in range(self.x_size):
                if v[i+2] > self.x_max[i]:
                    v[i+2] = self.x_max[i]
                elif v[i+2] < self.x_min[i]:
                    v[i+2] = self.x_min[i]
            
            self.evaluate_fitness(v) 
            #オオカミの位置を更新
            domination = self.evaluate_domination(x, v)
            if (domination == 1):
                self.EA[self.EA_size, 3:] = v
                self.EA_size += 1
                self.population[k] = v
            elif (domination == 0):
                self.EA[self.EA_size, 3:] = v
                self.EA_size += 1
                if (np.random.randint(0, 2) == 0):
                    self.population[k] = v


        #外部アーカイブを更新
        EA_next = np.zeros((self.wolf_size+self.max_EA_size, self.x_size+5))
        EA_next_size =0
        #f1の値で外部アーカイブを昇順にソート
        self.EA[:self.EA_size] = self.EA[:self.EA_size][np.argsort(self.EA[:self.EA_size, 3])]
        for i in range(self.EA_size):
            dominated = False
            f2_i = self.EA[i][4]
            for j in range(i):
                if (self.EA[j][4] < f2_i):
                    dominated = True
            if not dominated:
                EA_next[EA_next_size] = self.EA[i]
                EA_next_size += 1
        self.EA = EA_next
        self.EA_size = EA_next_size
        
        #crowd distanceを計算
        #f1の最近傍距離を計算
        self.EA[0][1] = 1e8
        self.EA[self.EA_size-1][1] = 1e8
        if self.EA_size > 3:
            EA_f1_size = max((self.EA[self.EA_size-1, 3]-self.EA[0, 3]), 1e-8)
            EA_f1_size = max(EA_f1_size, 1e-8)
            
            for i in range(1, self.EA_size-1):
                self.EA[i][1] = np.abs(self.EA[i-1][3]-self.EA[i+1][3])/EA_f1_size
                
        self.EA[:self.EA_size] = self.EA[:self.EA_size][np.argsort(self.EA[:self.EA_size, 4])]  
        self.EA[0][2] = 1e8
        self.EA[self.EA_size-1][2] = 1e8
        if self.EA_size > 3:
            EA_f2_size = max((self.EA[self.EA_size-1, 4]-self.EA[0, 4]), 1e-8)
            EA_f2_size = max(EA_f2_size, 1e-8)
            for i in range(1, self.EA_size-1):
                self.EA[i][2] = np.abs(self.EA[i-1][4]-self.EA[i+1][4])/EA_f2_size
        
        self.EA[:, 0] = self.EA[:, 1] + self.EA[:, 2]
        
        #外部アーカイブの大きさが容量を超えた場合
        if (self.EA_size > self.max_EA_size):
            #crowd distanceで外部アーカイブを降順にソート
            self.EA[:self.EA_size] = self.EA[:self.EA_size][np.argsort(-self.EA[:self.EA_size, 0])]
            #max_EA_size よりインデックスが大きいものを0にする
            for i in range(self.max_EA_size, self.EA_size):
                self.EA[i] = np.zeros(self.x_size+5)
            self.EA_size = self.max_EA_size
        self.generation+=1
              
    """評価値を計算""" 
    def evaluate_fitness(self, x):
        x[0], x[1] = self.func.calculate_func(x[2:])

    """優越性を評価"""
    def evaluate_domination(self, x1, x2):
        #x1がx2を優越
        if x1[0] <= x2[0] and x1[1] <= x2[1]:
            return -1 
        #x2がx1を優越
        if x1[0] >= x2[0] and x1[1] >= x2[1]:
            return 1
        #互いに優越されない
        return 0
    
    """EAのfunc1の値を取得"""
    def get_EA_f1(self):
        return self.EA[:self.EA_size,3]
    
    """EAのfunc2の値を取得"""
    def get_EA_f2(self):
        return self.EA[:self.EA_size,4]
    
    """集団のfunc1の値を取得"""
    def get_population_f1(self):
        return self.population[:,0]
    
    """集団のfunc2の値を取得"""
    def get_population_f2(self):
        return self.population[:,1]
    
        


if __name__ == "__main__":
    root = ctk.CTk()
    MOsimulator(root)
    root.mainloop()
    