# OS Project 1 Report

## 設計

我們使用min heap來實作priority queue，然後根據不同的排程方法，來傳相對應的參數，使我們每次extract min的時候可以得到目前priority最高的process。

這次的project，我們用到兩個cpu，一個跑main process，一個跑main process的child，這樣我們兩個process跑的時候就不會打架。

我們在main裡面，會根據input，看什麼時候有process ready，就把該process的資訊根據排程方法(fifo, rr, sjf, psjf)，丟進我們的min heap。每當我們要跑process的時候，就extract min然後執行該程式。

若我們需要中斷某個process，就用到sched_setscheduler，把他set成SCHED_IDLE，如果要讓他恢復執行，就set成SCHED_OTHER。

每當有程式執行完畢，會傳SIGCHLD給main process，我們有設計signal handler，來處理程式執行完後的事，並且看看還有沒有process正在等待，若有就繼續執行目前priority最高的process。

而我們使用clock_gettime加上CLOCK_REALTIME參數，來取得現在的時間。並且使用mmap，讓child process跟main process傳遞結束時間更方便。

## 結果

##### FIFO_1.txt

P1 74555
P2 74556
P3 74557
P4 74558
P5 74559

##### FIFO_2.txt

P1 74561
P2 74564
P3 74565
P4 74566

##### FIFO_3.txt

P1 74568
P2 74569
P3 74570
P4 74571
P5 74574
P6 74575
P7 74576

##### FIFO_4.txt

P1 74578
P2 74579
P3 74580
P4 74581

##### FIFO_5.txt

P1 74583
P2 74585
P3 74586
P4 74587
P5 74588
P6 74589
P7 74590

##### PSJF_1.txt

P4 74595
P3 74594
P2 74593
P1 74592

##### PSJF_2.txt

P2 74600
P1 74599
P4 74602
P5 74603
P3 74601

##### PSJF_3.txt

P2 74606
P3 74607
P4 74608
P1 74605

##### PSJF_4.txt

P3 74611
P2 74610
P4 74612
P1 74613

##### PSJF_5.txt

P1 74615
P3 74617
P2 74616
P4 74618
P5 74619

##### RR_1.txt

P1 74621
P2 74622
P3 74623
P4 74624
P5 74625

##### RR_2.txt

P1 74627
P2 74630

##### RR_3.txt

P3 74635
P1 74632
P2 74633
P6 74638
P5 74637
P4 74636

##### RR_4.txt

P4 74643
P5 74644
P6 74645
P3 74642
P7 74646
P2 74641
P1 74640

##### RR_5.txt

P4 74653
P5 74654
P6 74655
P3 74650
P7 74656
P2 74649
P1 74648

##### SJF_1.txt

P2 74659
P3 74660
P4 74661
P1 74662

##### SJF_2.txt

P1 74668
P2 74669
P3 74670
P4 74671
P5 74673

##### SJF_3.txt

P1 74675
P4 74676
P5 74677
P6 74678
P7 74679
P2 74680
P3 74681
P8 74682

##### SJF_4.txt

P1 74684
P2 74685
P3 74686
P5 74687
P4 74688

##### SJF_5.txt

P1 74690
P2 74700
P3 74701
P4 74702

## 比較

順序都是對的，不過跟理論值比較起來，main process 會多需要一些時間，大概每一萬 unit time 的理論值，main process 會多用幾十 unit time。

### 差異原因

因為我們的 main process 還需要判斷很多條件，並且也需要維護 priority queue，所以會花很多額外的時間。無法做到跟 child 一樣，完全只跑 unit time。

當我們使用 sched_setscheduler 的時候，是請 kernel 幫我們把 child process 的 priority 調整為 idle，中間會有一點時間差，並且 idle 是以極低的 priority 來跑，但會不會有時候還是被 CPU 排程到。

當 child process send SIGCHLD 的時候，會先交給 kernel 再傳給我們的 main process，所以這中間的傳遞過程，可能會有一寫誤差，我們不知道這個時間差多久。

## 各組員的貢獻

