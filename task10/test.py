import numpy as np
import random
import math
from matplotlib import pyplot as plt

"""
dimentionで関数の入力ベクトルの次元を指定できます
idx(0から4)で最適化に用いる関数を選択できます
実行するとPSO,GWO,CS,ASGCOの探索の様子をプロットしたグラフが出力されます
"""
dimension = 5
idx = 1 

# Rastrigin関数の定義
def rastrigin(x):
    A = 10
    return A * len(x) + sum((xi ** 2 - A * np.cos(2 * np.pi * xi)) for xi in x)

#Rosenbrock関数
def rosenbrock(x):
    return sum(100 * (x[i+1] - x[i]**2)**2 + (x[i] - 1)**2 for i in range(len(x)-1))

# Ackley関数の定義
def ackley(x):
    a, b, c = 20, 0.2, 2 * np.pi
    d = len(x)
    sum1 = np.sum(x ** 2)
    sum2 = np.sum(np.cos(c * x))
    return -a * np.exp(-b * np.sqrt(sum1 / d)) - np.exp(sum2 / d) + a + np.e

#Griewank関数
def griewank(x):
    sum_term = sum(x[i]**2 / 4000 for i in range(len(x)))
    prod_term = 1
    for i in range(len(x)):
        prod_term *= math.cos(x[i] / math.sqrt(i+1))
    return sum_term - prod_term + 1

# レヴィ飛行関数
def levy_flight(dim= 2, Lambda=1.5):
    sigma = (math.gamma(1 + Lambda) * np.sin(np.pi * Lambda / 2) /
             (math.gamma((1 + Lambda) / 2) * Lambda * 2 ** ((Lambda - 1) / 2))) ** (1 / Lambda)
    u = np.random.normal(0, sigma, size=dim)
    v = np.random.normal(0, 1, size=dim)
    return u / np.abs(v) ** (1 / Lambda)

# ASGCO アルゴリズム（GWO統合）
class ASGCO:
    def __init__(self, dim=2, pop_size=30, max_iter=100, w=0.5, c1=1.5, c2=1.5, P_levy=0.2, func = ackley, bounds = [-32.768, 32.768]  ):
        self.dim = dim  # 次元数
        self.pop_size = pop_size  # 個体数
        self.max_iter = max_iter  # 最大反復回数
        self.w = w  # 慣性係数
        self.c1, self.c2 = c1, c2  # PSOの学習係数
        self.P_levy = P_levy  # レヴィ飛行適用確率
        self.bounds = bounds
        self.func = func
        
    def optimize(self):
        # 個体の初期化
        pos = np.random.uniform(self.bounds[0], self.bounds[1], (self.pop_size, self.dim))
        vel = np.random.uniform(-1, 1, (self.pop_size, self.dim))
        pBest = pos.copy()
        pBest_scores = np.array([self.func(p) for p in pBest])
        gBest = pBest[np.argmin(pBest_scores)]
        
        #リーダーオオカミの初期化
        pbest_idx_sorted = np.argsort(pBest_scores)
        alpha = pBest[pbest_idx_sorted[0]]
        beta = pBest[pbest_idx_sorted[1]]
        delta = pBest[pbest_idx_sorted[2]]
        
        gBest_score = np.min(pBest_scores)
        
        gbests = []

        for t in range(self.max_iter):
            a = 2 - t * (2/ self.max_iter)  # GWOの収縮係数
            for i in range(self.pop_size):
                # PSO更新
                vel[i] = (self.w * vel[i] +
                          self.c1 * random.random() * (pBest[i] - pos[i]) +
                          self.c2 * random.random() * (gBest - pos[i]))
                pos[i] = pos[i] + vel[i]
                pos[i] = np.clip(pos[i], self.bounds[0], self.bounds[1])

                # GWOの適用
                A = 2 * a * np.random.rand(self.dim) - a
                C = 2 * np.random.rand(self.dim)
                D = np.abs(C * alpha - pos[i])
                pos1 = alpha - A * D
                A = 2 * a * np.random.rand(self.dim) - a
                C = 2 * np.random.rand(self.dim)
                D = np.abs(C * beta - pos[i])
                pos2 = beta - A * D
                A = 2 * a * np.random.rand(self.dim) - a
                C = 2 * np.random.rand(self.dim)
                D = np.abs(C * delta - pos[i])
                pos3 = delta - A * D
                pos[i] = (pos1+pos2+pos3)/3.0
                pos[i] = np.clip(pos[i], self.bounds[0], self.bounds[1])

                # 更新
                score = self.func(pos[i])
                if score < pBest_scores[i]:
                    pBest[i] = pos[i]
                    pBest_scores[i] = score

            # カッコウ探索のレヴィ飛行
            if random.random() < self.P_levy:
                idx = random.randint(0, self.pop_size - 1)
                new_pos = gBest + levy_flight(self.dim)
                new_pos = np.clip(new_pos, self.bounds[0], self.bounds[1])
                if self.func(new_pos) < self.func(pos[idx]):
                    pos[idx] = new_pos

            # グローバル最適解の更新
            new_gBest = pBest[np.argmin(pBest_scores)]
            new_gBest_score = np.min(pBest_scores)
            if new_gBest_score < gBest_score:
                gBest = new_gBest
                gBest_score = new_gBest_score

            gbests.append(gBest_score)

        return gbests, gBest_score
    

# GWO（グレイウルフ最適化）
class GWO:
    def __init__(self, dim=2, pop_size=30, max_iter=100, func = ackley, bounds = [-32.768, 32.768] ):
        self.dim = dim  # 次元数
        self.pop_size = pop_size  # 個体数
        self.max_iter = max_iter  # 最大反復回数
        self.bounds = bounds
        self.func = func

    def optimize(self):
        # 初期化
        wolves = np.random.uniform(self.bounds[0], self.bounds[1], (self.pop_size, self.dim))
        scores = np.array([self.func(w) for w in wolves])
        alpha, beta, delta = wolves[np.argsort(scores)[:3]]
        gbest = scores.min()

        gbests = []
        for t in range(self.max_iter):
            a = 2 - t * (2 / self.max_iter)
            for i in range(self.pop_size):
                A1, A2, A3 = 2 * a * np.random.rand(self.dim) - a, 2 * a * np.random.rand(self.dim) - a, 2 * a * np.random.rand(self.dim) - a
                C1, C2, C3 = 2 * np.random.rand(self.dim), 2 * np.random.rand(self.dim), 2 * np.random.rand(self.dim)
                D_alpha, D_beta, D_delta = np.abs(C1 * alpha - wolves[i]), np.abs(C2 * beta - wolves[i]), np.abs(C3 * delta - wolves[i])
                X1, X2, X3 = alpha - A1 * D_alpha, beta - A2 * D_beta, delta - A3 * D_delta
                wolves[i] = (X1 + X2 + X3) / 3
                wolves[i] = np.clip(wolves[i], self.bounds[0], self.bounds[1])

            scores = np.array([self.func(w) for w in wolves])
            alpha, beta, delta = wolves[np.argsort(scores)[:3]]
            gbest = min(gbest, scores.min())

            gbests.append(gbest)

        return gbests,scores.min()

# PSO（粒子群最適化）
class PSO:
    def __init__(self, dim=2, pop_size=30, max_iter=100, w=0.7, c1=1.5, c2=1.5, func = ackley, bounds = [-32.768, 32.768] ):
        self.dim = dim  # 次元数
        self.pop_size = pop_size  # 粒子数
        self.max_iter = max_iter  # 最大反復回数
        self.w = w  # 慣性係数
        self.c1, self.c2 = c1, c2  # 学習係数
        self.bounds = bounds
        self.func = func

    def optimize(self):
        # 粒子の初期化
        pos = np.random.uniform(self.bounds[0], self.bounds[1], (self.pop_size, self.dim))
        vel = np.random.uniform(-1, 1, (self.pop_size, self.dim))
        pBest = pos.copy()
        pBest_scores = np.array([self.func(p) for p in pBest])
        gBest = pBest[np.argmin(pBest_scores)]
        gBest_score = np.min(pBest_scores)
        
        gbests = []

        for t in range(self.max_iter):
            for i in range(self.pop_size):
                vel[i] = (self.w * vel[i] +
                          self.c1 * random.random() * (pBest[i] - pos[i]) +
                          self.c2 * random.random() * (gBest - pos[i]))
                pos[i] = pos[i] + vel[i]
                pos[i] = np.clip(pos[i], self.bounds[0], self.bounds[1])

                score = self.func(pos[i])
                if score < pBest_scores[i]:
                    pBest[i] = pos[i]
                    pBest_scores[i] = score

            new_gBest = pBest[np.argmin(pBest_scores)]
            new_gBest_score = np.min(pBest_scores)
            if new_gBest_score < gBest_score:
                gBest = new_gBest
                gBest_score = new_gBest_score

            gbests.append(gBest_score)

        return gbests, gBest_score
    
# CS（カッコウ探索）アルゴリズム
class CS:
    def __init__(self, dim=2, pop_size=30, max_iter=100, func=None, bounds=[-32.768, 32.768], pa=0.25):
        self.dim = dim  # 次元数
        self.pop_size = pop_size  # 巣の数（個体数）
        self.max_iter = max_iter  # 最大反復回数
        self.func = func  # 最適化関数
        self.bounds = bounds  # 探索範囲
        self.pa = pa  # 放棄率

    def optimize(self):
        # 初期化
        nests = np.random.uniform(self.bounds[0], self.bounds[1], (self.pop_size, self.dim))
        scores = np.array([self.func(n) for n in nests])
        best_idx = np.argmin(scores)
        gBest = nests[best_idx]
        gBest_score = scores[best_idx]

        gbests = []
        for t in range(self.max_iter):
            new_nests = nests.copy()

            # レヴィ飛行による更新
            for i in range(self.pop_size):
                step = levy_flight(dim=self.dim)
                new_nest = nests[i] + step * (nests[i] - gBest)
                new_nest = np.clip(new_nest, self.bounds[0], self.bounds[1])
                if self.func(new_nest) < scores[i]: 
                    new_nests[i] = new_nest

            # 巣の放棄（一定確率でランダム更新）
            rand_indices = np.random.rand(self.pop_size) < self.pa
            new_nests[rand_indices] = np.random.uniform(self.bounds[0], self.bounds[1], (np.sum(rand_indices), self.dim))

            # 新しいスコアを計算
            scores = np.array([self.func(n) for n in new_nests])
            best_idx = np.argmin(scores)
            if scores[best_idx] < gBest_score:
                gBest = new_nests[best_idx]
                gBest_score = scores[best_idx]

            nests = new_nests
            gbests.append(gBest_score)

        return gbests, gBest_score


# 最適化の実行

funcs = [ackley, rosenbrock, rastrigin, griewank]
func_names = ["Ackley", "Rosenbrock", "Rastrigin", "Griewank"]
borders = [[-32.768, 32.768], [-2.048, 2.048], [-5.12, 5.12],[-600,600]]


optimizer = ASGCO(dimension, func = funcs[idx], bounds=borders[idx],max_iter=100)
bests1, best_value = optimizer.optimize()


optimizer = GWO(dimension, func = funcs[idx], bounds=borders[idx],max_iter=100)
bests2, best_value = optimizer.optimize()



optimizer = PSO(dimension, func = funcs[idx], bounds=borders[idx],max_iter=100)
bests3, best_value = optimizer.optimize()


optimizer = CS(dimension, func = funcs[idx], bounds=borders[idx],max_iter=100)
bests4, best_value = optimizer.optimize()

min_val = np.min(np.concatenate((bests1, bests2, bests3, bests4)))*1.1
max_val= np.max(np.concatenate((bests1, bests2, bests3, bests4)))*0.9


plt.figure(figsize=(10, 7))
plt.subplot(2, 2, 1)
plt.semilogy(bests1)
plt.title("ASGCO", )
plt.ylabel("min value")
plt.ylim([min_val, max_val])

plt.subplot(2, 2, 2)
plt.semilogy(bests2)
plt.title("GWO")
plt.ylim([min_val, max_val])


plt.subplot(2, 2, 3)
plt.semilogy(bests3)
plt.ylim([min_val, max_val])
plt.xlabel("generation")
plt.ylabel("min value")
plt.title("PSO")

plt.subplot(2, 2, 4)
plt.semilogy(bests4)
plt.ylim([min_val, max_val])
plt.title("CS")
plt.xlabel("generation")

plt.show()