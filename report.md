# OS Project 1 Report

## 設計

我們使用min heap來實作priority queue，然後根據不同的排程方法，來傳相對應的參數，使我們每次extract min的時候可以得到目前priority最高的process。

這次的project，我們用到兩個cpu，一個跑main process，一個跑main process的child，這樣我們兩個process跑的時候就不會打架。

我們在main裡面，會根據input，看什麼時候有process ready，就把該process的資訊根據排程方法(fifo, rr, sjf, psjf)，丟進我們的min heap。每當我們要跑process的時候，就extract min然後執行該程式。

若我們需要中斷某個process，就用到sched_setscheduler，把他set成SCHED_IDLE，如果要讓他恢復執行，就set成SCHED_OTHER。

每當有程式執行完畢，會傳SIGCHLD給main process，我們有設計signal handler，來處理程式執行完後的事，並且看看還有沒有process正在等待，若有就繼續執行目前priority最高的process。

而我們使用clock_gettime加上CLOCK_REALTIME參數，來取得現在的時間。並且使用mmap，讓child process跟main process傳遞結束時間更方便。

## 結果



## 比較

順序都是對的，不過跟理論值比較起來，main process 會多需要一些時間，大概每一萬 unit time 的理論值，main process 會多用幾十 unit time。

### 差異原因

因為我們的 main process 還需要判斷很多條件，並且也需要維護 priority queue，所以會花很多額外的時間。無法做到跟 child 一樣，完全只跑 unit time。

當我們使用 sched_setscheduler 的時候，是請 kernel 幫我們把 child process 的 priority 調整為 idle，中間會有一點時間差，並且 idle 是以極低的 priority 來跑，但會不會有時候還是被 CPU 排程到。

當 child process send SIGCHLD 的時候，會先交給 kernel 再傳給我們的 main process，所以這中間的傳遞過程，可能會有一寫誤差，我們不知道這個時間差多久。

## 各組員的貢獻

