% CSVファイルの読み込み
data = readtable('output2.csv');
data1 = readtable('output3.csv');
data2 = readtable('output4.csv'); % output4.csvを読み込む

% 'L'と'answerlength'を抽出
L = data.L;
answerlength = data.answer_length;
answerlength1 = data1.answer_length;
answerlength2 = data2.answer_length; % output4.csvのanswer_lengthを追加

% グラフのプロット (解の長さ)
figure('Position', [0, 0, 600, 400]); % 新しい図を作成
plot(L, answerlength, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold on;
plot(L, answerlength1, '-o', 'LineWidth', 2, 'MarkerSize', 6);
plot(L, answerlength2, '-o', 'LineWidth', 2, 'MarkerSize', 6); % output4.csvのデータを追加
hold off;

% 目盛り以外のフォントサイズを設定
set(gca, 'FontSize', 20); % 軸ラベル、タイトル、凡例以外のフォントサイズを変更

% 目盛りのフォントサイズを個別に設定
ax = gca;
ax.XAxis.FontSize = 12; % X軸の目盛りフォントサイズ
ax.YAxis.FontSize = 12; % Y軸の目盛りフォントサイズ

% レジェンドのフォントサイズを設定
hLegend = legend({"$f_2(n)$", "$f_3(n)$", "$f_4(n)$"}, 'Interpreter', 'latex', 'Location', 'northwest');
set(hLegend, 'FontSize', 20);

% グラフのラベル付け
xlabel('$L$', 'FontSize', 20, 'Interpreter', 'latex'); % 横軸のラベル
ylabel('解の長さ', 'FontSize', 20); % 縦軸のラベル
ylim([20, 55]);
saveas(gcf, 'answer_length1.png');

% 'num_child'の抽出
numchild = data.num_child;
numchild1 = data1.num_child;
numchild2 = data2.num_child; % output4.csvのnum_childを追加

% グラフのプロット (子ノードの数)
figure('Position', [0, 0, 600, 400]); % 新しい図を作成
plot(L, numchild, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold on;
plot(L, numchild1, '-o', 'LineWidth', 2, 'MarkerSize', 6);
plot(L, numchild2, '-o', 'LineWidth', 2, 'MarkerSize', 6); % output4.csvのデータを追加
hold off;

% 目盛り以外のフォントサイズを設定
set(gca, 'FontSize', 20); % 軸ラベル、タイトル、凡例以外のフォントサイズを変更

% 目盛りのフォントサイズを個別に設定
ax = gca;
ax.XAxis.FontSize = 12; % X軸の目盛りフォントサイズ
ax.YAxis.FontSize = 12; % Y軸の目盛りフォントサイズ
set(gca, 'YScale', 'log'); % Y軸を片対数スケールに設定

% レジェンドのフォントサイズを設定
hLegend = legend({"$f_2(n)$", "$f_3(n)$", "$f_4(n)$"}, 'Interpreter', 'latex', 'Location', 'northwest',  'Orientation', 'horizontal');
set(hLegend, 'FontSize', 20);

% グラフのラベル付け
xlabel('$L$', 'FontSize', 20, 'Interpreter', 'latex'); % 横軸のラベル
ylabel('子ノードの数', 'FontSize', 20); % 縦軸のラベル
ylim([1e3, 5e6]);

saveas(gcf, 'numchild1.png');
