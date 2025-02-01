data = readtable('output2.csv');
data1 = readtable('output3.csv');
% 'L'と'answerlength'を抽出
L = data.L;
answerlength = data.answer_length;
answerlength1 = data1.answer_length;

% グラフのプロット
figure('Position', [0, 0, 600, 400]); % 新しい図を作成
plot(L, answerlength, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold on;
plot(L, answerlength1, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold off;

% 目盛り以外のフォントサイズを設定
set(gca, 'FontSize', 20); % 軸ラベル、タイトル、凡例以外のフォントサイズを変更

% 目盛りのフォントサイズを個別に設定
ax = gca;
ax.XAxis.FontSize = 12; % X軸の目盛りフォントサイズ
ax.YAxis.FontSize = 12; % Y軸の目盛りフォントサイズ

% レジェンドのフォントサイズを設定
hLegend = legend({"$f_2(n)$", "$f_3(n)$"}, 'Interpreter', 'latex','Location', 'northwest');
set(hLegend, 'FontSize', 20);

% グラフのラベル付け
xlabel('$L$', 'FontSize', 20, 'Interpreter', 'latex'); % 横軸のラベル
ylabel('解の長さ', 'FontSize', 20); % 縦軸のラベル
ylim([15, 55]);
saveas(gcf, 'answer_length.png');

numchild = data.num_child;
numchild1 = data1.num_child;

% グラフのプロット
figure('Position', [0, 0, 600, 400]); % 新しい図を作成
plot(L, numchild, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold on;
plot(L, numchild1, '-o', 'LineWidth', 2, 'MarkerSize', 6); hold off;

% 目盛り以外のフォントサイズを設定
set(gca, 'FontSize', 20); % 軸ラベル、タイトル、凡例以外のフォントサイズを変更

% 目盛りのフォントサイズを個別に設定
ax = gca;
ax.XAxis.FontSize = 12; % X軸の目盛りフォントサイズ
ax.YAxis.FontSize = 12; % Y軸の目盛りフォントサイズ
set(gca, 'YScale', 'log')
% レジェンドのフォントサイズを設定
hLegend = legend({"$f_2(n)$", "$f_3(n)$"}, 'Interpreter', 'latex','Location', 'northwest',  'Orientation', 'horizontal');
set(hLegend, 'FontSize', 20);

% グラフのラベル付け
xlabel('$L$', 'FontSize', 20, 'Interpreter', 'latex'); % 横軸のラベル
ylabel('子ノードの数', 'FontSize', 20); % 縦軸のラベル
ylim([5000, 3e6]);

saveas(gcf, 'numchild.png');