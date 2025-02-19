# 人工知能中間レポート 課題5 プログラムの説明

ABCアルゴリズムを用いてHP構造予測問題を解くプログラムを作成した．


## コンパイル

ディレクトリに移動したうえで，

```bash
$ make
```

と入力するとコンパイルできる．このプログラムでは結果の描画に OpenGL の GLUT を用いる．GLUTがインストールされていない場合は，インストールする必要がある．GLUTは，

```bash
$ sudo apt-get install freeglut3 freeglut3-dev
```

とするとインストールできる．

## 実行

コンパイルしたら

```bash
$ bin/main
```

でプログラムを実行できる．シミュレーションを開始する前に，まず最適化したHP配列とＡＢＣアルゴリズムのパラメーターをいくつか設定する必要がある．実行すると

```
This is a program that searches for the optimal HP lattice structure using the ABC algorithm

Please configure the simulation settings. If you want to use the default value, just press Enter.
? Enter the input array (default = H10H2(PH)3(P3)2HPH4(PH)3PH2): 
```
のように設定画面がターミナルに表示される．入力してEnterをクリックすると設定できる．何も入力せずにEnterをクリックすると`(default= )`で示されているデフォルトのものが使われる．設定内容は以下の通り．

- `Enter the input array (default = H10H2(PH)3(P3)2HPH4(PH)3PH2):` <br>
HP配列を入力する．

- `Enter the number of generations (default=100):` <br>
シミュレーションの世代数を入力する．

- `Enter the population size (default = 1000):` <br>
ハチの個体数を入力する．

- `Enter the max trials for Scout Bee (default = 50): `<br>
Scout Beeになるまでの試行回数の上限を入力する．

不適切な入力がされた場合には下記のようにエラーになり再入力を要求される．

```text
例1
? Enter the input array (default = H10H2(PH)3(P3)2HPH4(PH)3PH2): 
Error: Syntax error detected. Please enter a valid input array.
          aH3
          ^
```

```text
例2 
✔ Dimension for the optimization: 2D
✔ Input array: H10H2(PH)3(P3)2HPH4(PH)3PH2
                    -> HHHHHHHHHHHHPHPHPHPPPPPPHPHHHHPHPHPHPHH
✔ Generations: 100
✔ Population size: 1000
? Enter the max trials for Scout Bee (default = 50): 
Error: Invalid input. Please enter a positive integer for max trials.
```

すべての設定が完了すると以下のようにシミュレーションが開始され，今の世代と今までに見つかった水素結合の数の最大値が表示される．
．
```
✔ Input array: H10H2(PH)3(P3)2HPH4(PH)3PH2
                    -> HHHHHHHHHHHHPHPHPHPPPPPPHPHHHHPHPHPHPHH
✔ Generations: 100
✔ Population size: 1000
✔ Max trials for Scout Bee: 50

  
*****Simulation start*******
 Generation : 69
 Maximum hydrogen bonds found: 26
```

設定した世代数に到達するとシミュレーションが終了し，見つかった水素結合の数の最大値とシミュレーションの実行時間が次のように表示される．

```bash
*****Simulation finish*******
 Generation : 100
 Maximum hydrogen bonds found: 27
 Elapsed time : 12.2044 s
```

そして，新しくウィンドウが開き，探索で見つけた最大の水素結合数を持つHP構造が表示される．ウィンドウの操作方法と見かたは以下の通り．

- 赤色の球 : 疎水性アミノ酸，青色の球 : 親水性アミノ酸
- 赤色の線 : ペプチド結合，青色の点線 : 水素結合
- 左クリックをしながらマウスを動かすと視点を変更
- 右クリックをしながらマウスを動かすと画面を拡大縮小
- 画面右上の X マークをクリックすると画面が閉じてプログラムが終了
  
## ファイルの説明



- ```vec3.h``` : 3次元ベクトルのクラス
- ```display.h```, ```display.cpp``` : OpenGLでの描画
- ```decode.h```, ```decode.cpp``` : 入力配列を処理しやすい形に変形
- ```setting.h```, `setting.cpp` : 設定画面の処理

- ```main.c``` : main関数