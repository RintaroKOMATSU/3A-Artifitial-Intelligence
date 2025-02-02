# 課題12
MOABCとMOGWOで多目的最適化をシミュレートできます．

## 実行方法
シミュレーターはpythonで書かれています．実行にはpython3のインストールが必要です．また， ```numpy```, ```matplotlib```, ```tkinter```, ```custom tkinter```もインストールされている必要があります．．インストールしていない場合は以下のコマンドでインストールします．

```bash
pip install numpy matplotlib tk customtkinter
```
必要なライブラリをインストールしたうえで以下のコマンドで実行することができます
```
python3 MOsimulator.py
```

## ファイル構成
- `MOsimulator.py`: メインのシミュレーションプログラム
- `README.md`: このファイル
- `functions` : 目的関数のデータがdat形式で保存されています．これらの関数は File → Load Function Setting から読み込むことができます．レポートの6つのベンチマーク関数にはそれぞれ p1*.dat から p6*.datが対応しています．

## 使用方法
詳しくはレポートに書いてありますが，File → Load Function Settingからfunctionsの中のどれかを読み込んで，右下の丸いボタンを押せばシミュレーションが開始します，アルゴリズムは右上の設定のところからMOGWOとMOABCのどちらかを指定できます．

## 注意事項
手元のwindowsで作成しましたが，一応ECCS端末のMacとWSL上のlinux環境でも動くことを確認していますが，デバイスによっては画面が大きすぎたり，文字が小さすぎたりとか変な感じになってしまうかもしれないです．

