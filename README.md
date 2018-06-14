# NTCU smart campus project on Cooridinate Beanforming protocol design with MrLoop WiGig dongle.

The goal of this project is for NCTU smart campus demo project.

### Information:
- Using Mrloop WiGig dongle to transfer RF signal
- Implementation the beanforming protocol for exhausting search and smart search by Machine learning.
- Coordinate beansearch for mutiple WiGig dongle
- handover on Wifi when WiGig dongle can't recieve signal.

### platforms:
- Ubuntu of Linux kernal 4.4.0

### Known issues:
- Sector mode and Omit mode need to be check that can be run at the same time.

### To Be Done:
- System design:
- [ ] RSSI data collection
- [ ] Central controling server
- [ ] Flow of the system
- Protocol Design:
- [ ] protocol header for WiGig bean selection
- [ ] protocol header for WiGig to dontroling server.
- Exhausting Search:
- [ ] Iteration for Tx to Rx
- [ ] Iteration for Rx to Tx
- Machine Learning:
- [ ] Q learning

### Change log:

#### 2018/6/14
First commit on this project.

#### 2018/6/14 11:03
Add the trasmittion header file.

