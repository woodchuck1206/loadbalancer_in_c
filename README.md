# Logical Load Balancer implementation (in progress)

## Performance
### 2020-09-28 14시 40분
[이슈 해결](#done-1) 후 localhost에서 10,000rps+ 정상 동작 확인, 실제 aws vpn 상에서 확인해볼 것

### 2020-09-28 10시
Repo의 Python 스크립트를 사용해서 테스트 할 때, 대략 1000 requests/s가 현재(2020-09-28) 한계인 것으로 보입니다.\
rps(Request per Second)가 한계를 넘었을 때, 로드밸런서는 (대체로) 죽습니다. 수정 중 입니다.

Using the provided python script, roughly 1000 requests/sec seesm to the limit.\
When rps(Request per Second) exceeds the limit, the load balancer dies. Needs improvement.

## Observations
### Issue 1
2020-09-28 기준 LB는 동시적인(interval \~0.001s) request 발생 시 shutdown 되는 현상이 있습니다.\
0.0015\~초 정도의 인터벌이 있을때, LB가 shutdown 되는 모습은 관찰되지 않았습니다.

As of 28th Sep, 2020, The load balancer shuts down when there are simultaneous requests(interval ~0.001s).\
However, with little interval(0.0015s~) between requests, the lb does not seem to shut down.

## Done
### Done 1
2020-09-28 Recv socket 변수의 스코프가 끝나고, 다음 스코프가 왔을 때, 아직 스레드가 끝나지 않았는데 동일한 주소를 공유하는 변수의 value가 바뀌어서 한 FD에 대한 Racing Condition이 발생했음.\
Socket placeholder 배열을 생성하고 이 배열을 사용해서 recv socket을 관리하여 [Issue 1](#issue-1) 해결

### Done 2
Real Time Endpoint Update API 추가, 하지만 start의 가장 앞에 수정하고 싶은 endpoint가 있는 경우를 위해서 struct Endpoint \*start가 아닌 struct Endpoint \*\*start를 받아야 할 필요가 있음. 더 좋은 방법이 있는지 생각해볼것. [To Do:Real Time Endpoint Update](#real-time-endpoint-update) 기능 추가, 기능 수정 필요

## TO DO
### Real Time Endpoint Update
특정한 format을 따르는 요청을 보내면 엔드포인트를 재설정(추가/제거)한다.
Format/Designated Port
Format 

    ADD 127.0.0.1   9876    //Tab으로 나눠진 RAW TEXT
    DEL 127.0.0.1   9876    //Method/HOST/PORT로 나뉜다. 이미 존재하는 엔드포인트를 추가 / 없는 엔드포인트를 제거 요청은 무시한다.


