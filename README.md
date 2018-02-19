12ステップで作る 組み込みOS自作入門 勉強用リポジトリ
===============

# 概要

[これ](https://www.amazon.co.jp/12%E3%82%B9%E3%83%86%E3%83%83%E3%83%97%E3%81%A7%E4%BD%9C%E3%82%8B%E7%B5%84%E8%BE%BC%E3%81%BFOS%E8%87%AA%E4%BD%9C%E5%85%A5%E9%96%80-%E5%9D%82%E4%BA%95-%E5%BC%98%E4%BA%AE/dp/4877832394)と[ここ](http://kozos.jp/books/makeos/)を使って組込プログラムの勉強する


# ディップスイッチ設定

|    | 1  | 2   | 3   | 4   |
|----|----|-----|-----|-----|
|書込| ON | ON  | OFF | ON  |
|実行| ON | OFF | ON  | OFF |

# 注意点

+ WSLでやる場合は、毎回シリアルポートの権限を変える必要がある
+ ポート設定が正しいのに書き込めない場合はリセットすると書き込めるときもある