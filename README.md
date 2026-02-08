# TCP/UDP Routing 網路模擬系統

## 專案概述

本專案實現了一個 TCP 和 UDP 封包路由的網路模擬系統，用於分析網路封包的傳輸效能，包括端到端延遲（End-to-End Delay）、排隊延遲（Queuing Delay）、等待時間（Waiting Time）和吞吐量（Throughput）等網路效能指標。

系統由三個主要組件構成：
- **Client（客戶端）**：發送 TCP 封包並接收 UDP 封包
- **Router（路由器）**：接收並轉發 TCP/UDP 封包，計算排隊延遲
- **Server（伺服器）**：接收 TCP 封包並發送 UDP 封包

## 系統架構

```
Client (127.0.0.1:9003)
    ↓ TCP
Router (127.0.0.3:9002)
    ↓ TCP
Server (127.0.0.2:9000)
    ↓ UDP
Router (127.0.0.3:9002)
    ↓ UDP
Client (127.0.0.1:9003)
```

## 檔案說明

### client.c
客戶端程式，主要功能：
- 使用 TCP 連接發送封包至 Router
- 建構完整的封包結構（MAC Header + IP Header + TCP Header + Payload）
- 接收從 Server 經由 Router 傳回的 UDP 封包
- 計算 UDP 封包的端到端延遲
- 計算傳輸吞吐量

### router.c
路由器程式，主要功能：
- 作為 TCP 連接的中繼節點
- 接收來自 Client 的 TCP 封包並轉發至 Server
- 接收來自 Server 的 UDP 封包並轉發至 Client
- 計算並記錄系統的排隊延遲（Queuing Delay）
- 分析等待時間（Waiting Time）和服務時間（Service Time）
- 使用歷史平均法計算平均排隊延遲

### server.c
伺服器程式，主要功能：
- 接收來自 Router 的 TCP 封包
- 計算 TCP 封包的端到端延遲
- 發送 UDP 封包回 Client（經由 Router）
- 計算傳輸吞吐量

## 封包結構

### MAC Header (18 bytes)
```c
- Source MAC Address: 6 bytes
- Destination MAC Address: 6 bytes
- Frame Type: 2 bytes
- CRC: 4 bytes
```

### IP Header (20 bytes)
```c
- Version & IHL: 1 byte
- Type of Service: 1 byte
- Total Length: 2 bytes
- Identification: 2 bytes
- Flags & Fragment Offset: 2 bytes
- Time to Live (TTL): 1 byte
- Protocol: 1 byte (0x06=TCP, 0x11=UDP)
- Header Checksum: 2 bytes
- Source IP: 4 bytes
- Destination IP: 4 bytes
- Options: 4 bytes
```

### TCP Header (20 bytes)
```c
- Source Port: 2 bytes
- Destination Port: 2 bytes
- Sequence Number: 4 bytes
- Acknowledgment Number: 4 bytes
- Offset, Reserved & Flags: 2 bytes
- Window Size: 2 bytes
- Checksum: 2 bytes
- Urgent Pointer: 2 bytes
```

### UDP Header (8 bytes)
```c
- Source Port: 2 bytes
- Destination Port: 2 bytes
- Segment Length: 2 bytes
- Checksum: 2 bytes
```

## 網路參數設定

```c
#define MTU 1500                    // 最大傳輸單元
#define PACKET_SIZE 1518            // 封包大小
#define CLIENT_IP "127.0.0.1"       // 客戶端 IP
#define SERVER_IP "127.0.0.2"       // 伺服器 IP
#define ROUTER_IP "127.0.0.3"       // 路由器 IP
#define SERVER_PORT 9000            // 伺服器埠號
#define ROUTER_PORT 9002            // 路由器埠號
#define CLIENT_PORT 9003            // 客戶端埠號
```

## 編譯與執行

### 編譯

```bash
# 編譯 Client
g++ -o client client.c -lpthread

# 編譯 Router
g++ -o router router.c -lpthread

# 編譯 Server
g++ -o server server.c -lpthread
```

### 執行順序

**重要：請依照以下順序啟動程式**

1. 首先啟動 Server：
```bash
./server
```

2. 然後啟動 Router：
```bash
./router
```

3. 最後啟動 Client：
```bash
./client
```

## 效能測量指標

### 1. 端到端延遲（End-to-End Delay）
- 從封包發送到接收的總時間
- 使用 timestamp 機制測量
- 單位：微秒（μs）

### 2. 排隊延遲（Queuing Delay）
- 包含等待時間和服務時間
- 公式：Queuing Delay = Waiting Time + Service Time
- 使用指數加權移動平均（EWMA）計算平均值
- 權重：歷史值 70%，當前值 30%

### 3. 等待時間（Waiting Time）
- 封包在佇列中等待處理的時間
- 計算方式：接收時間 - 發送時間

### 4. 服務時間（Service Time）
- 封包在系統中處理的時間
- Router 中的 TCP 服務時間：2000ms（可調整）
- Router 中的 UDP 服務時間：3000ms（可調整）

### 5. 吞吐量（Throughput）
- 單位時間內傳輸的資料量
- 單位：Mbps（百萬位元/秒）
- 測量範圍：完整的封包傳輸過程

## 程式特點

### 多執行緒設計
- 使用 C++ `std::thread` 實現並行處理
- TCP 和 UDP 傳輸同時進行
- 使用 `std::mutex` 防止資源競爭

### 時間戳記機制
- 使用 `gettimeofday()` 獲取高精度時間
- 時間精度：微秒（μs）
- 時間戳記嵌入封包 buffer 中傳輸

### 封包數量控制
- 預設發送/接收 10 個封包
- 可在程式中調整 `while(cnt<10)` 的數值

## 輸出範例

### Router 輸出
```
---router 從收到client送的到傳送至server時間分析----
system time2: 1234567890 usec
systemtime3: 1234569890 usec
waiting time: 1000 usec
service time: 2000000 usec
queuing delay: 2001000 usec
avg queuing delay: 600300 usec
router send tcp packet to server
```

### Server 輸出
```
-----server receive-----
payload : abcd
TCP end_to_end_delay: 2050000usec
server end_to_end_delay timestamp: 1234569890usec
```

### Client 輸出
```
client send tcp packet
client rcv UDP packet 1 !
UDP end_to_end_delay: 5100000usec
throughput of client: 0.023438 mbps
```

## 系統需求

- **作業系統**：Linux / Unix-like 系統
- **編譯器**：支援 C++11 的 g++
- **必要函式庫**：
  - pthread（多執行緒）
  - socket（網路通訊）
  - arpa/inet（網路位址轉換）

## 注意事項

1. **IP 位址設定**：程式使用 loopback 位址進行本機測試
2. **埠號衝突**：確保使用的埠號未被其他程式佔用
3. **執行順序**：必須依照 Server → Router → Client 的順序啟動
4. **延遲時間**：Router 中的 `usleep()` 可調整以模擬不同的網路延遲
5. **封包數量**：預設為 10 個封包，可依需求調整

## 研究用途

本專案適用於以下研究領域：
- 網路效能分析
- TCP/UDP 協定比較
- 排隊理論驗證
- 網路延遲測量
- 吞吐量分析

## 授權

本專案為碩士論文研究程式碼，供學術研究使用。

---

**最後更新**：2026年2月
