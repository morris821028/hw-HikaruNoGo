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