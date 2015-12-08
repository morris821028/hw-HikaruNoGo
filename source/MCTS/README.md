## README ##

### 手動編譯 ###

```
$ g++ -O3 Stephanie.cpp board.cpp MCTS.cpp
```

### Makefile ###

#### 編譯 ####

```
make all
```

#### GUI 上測試 ####

```
make guiplay
```

#### 背景下自動測試 ####

```
make play
```

測試場數按照 Makefile 中 `GAME_NUM = 1` 定義，對局結果在 `./test/result/test.dat` 和數場的 `./test/result/test-X.sgf`，利用 GoGUI 打開後，可以直接觀看到對局結果。

藉由 GoGUI 顯示 .sgf 時，交替對局的結果並不是開當下的白子或黑子。例如第一局結果為：

```
Black command: Stephanie.exe
White command: randomgoTA.exe
Black version: 1.02
White version: 1.02
Result[Black]: B+38
Result[White]: B+38
Host: morris821028-PC
Date: December 8, 2015 9:40:25 PM CST
```

第二局結果為：

```
Black command: randomgoTA.exe
White command: Stephanie.exe
Black version: 1.02
White version: 1.02
Result[Black]: B+88
Result[White]: B+88
Host: morris821028-PC
Date: December 8, 2015 9:41:13 PM CST
```

但是在 console 最後輸出卻是 `W+88` 這一點保持疑惑。



#### 清除所有執行檔 ####

```
make clean
```

### 效能分析 ###

編譯時期加入參數 `g++ -pg`，確切將執行檔執行過一次後會產生 `gmon.out`，接著執行 

```
$ gprof Stephanie.exe gmon.out >analysis.txt
```

從 `analysis.txt` 看到大部份的時間卡在哪一個函數中，借以進行效能改善。