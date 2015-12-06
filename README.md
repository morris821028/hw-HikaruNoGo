## Go ##

圍棋對局

### 安裝步驟 ###

* 安裝 [GoGUI](http://gogui.sourceforge.net/)，有可能需要額外安裝 java。
* 開啟 GoGUI 並且在工具列 GoGUI > Program > New Program > Command 中把編寫好的執行檔放入。命名為萌萌哒的史蒂芙 Stephanie，接著 Program > Attach > Stephanie 開啟。
* 關閉則按下 Program > Detach。

### 其他 ###

若要雙人對戰，則在 command line 下，執行 

```
morris821028@morris821028-PC MINGW64 ~/Desktop/GoGui
$ ./gogui -program "gogui-twogtp -white randomgo_ta.exe -black randomgo_tam.exe -size 9" -computer-both
```

若要 debug 寫的程序

```
morris821028@morris821028-PC MINGW64 ~/Desktop/github/hw-HikaruNoGo/source/MCTS (master)
$ ./Stephanie.exe -display
```

接著藉由 GTP protocol 進行互動

* `clear_board` 清空盤面
* `genmove b` 要求程序決策黑子要下哪一步。同樣地 `genmove w` 決策白子。
* `play w D4` 則表示指定白子下在 D4 的位子。

## 參考 ##

由於沒修課，只好上網搜中文論文，了解概念用途，並沒有完全實作細節。

* [電腦暗棋對局程式之設計與製作 - 范綱宇 陳志昌 中原大學](http://www.csie.ntnu.edu.tw/tcga2014/program-agenda/tcga2014_submission_4%EF%BC%BF%E7%B6%B1%E5%AE%87%20%E8%8C%83%20and%20%E5%BF%97%E6%98%8C%20%E9%99%B3.%20%E9%9B%BB%E8%85%A6%E6%9A%97%E6%A3%8B%E5%B0%8D%E5%B1%80%E7%A8%8B%E5%BC%8F%E4%B9%8B%E8%A8%AD%E8%A8%88%E8%88%87%E8%A3%BD%E4%BD%9C.pdf)
* [一個蒙地卡羅之電腦圍棋程式之設計 - 陳俊嶧](https://ir.nctu.edu.tw/bitstream/11536/45925/1/558001.pdf)