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
 
