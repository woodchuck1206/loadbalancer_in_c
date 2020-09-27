# Logical Load Balancer implementation (in progress)

## Performance
Repo의 Python 스크립트를 사용해서 테스트 할 때, 대략 1000 requests/s가 현재(2020-09-28) 한계인 것으로 보입니다.\
rps(Request per Second)가 한계를 넘었을 때, 로드밸런서는 (대체로) 죽습니다. 수정 중 입니다.\

Using the provided python script, roughly 1000 requests/sec seesm to the limit.\
When rps(Request per Second) exceeds the limit, the load balancer dies. Needs improvement.

## Observations
2020-09-28 기준 LB는 동시적인 request에 취약한 것으로 보입니다.\
0.002~초 정도의 인터벌이 있을때, LB가 shutdown 되는 모습은 관찰되지 않았습니다.\

As of 28th Sep, 2020, The load balancer seems to be vulnerable to simultaneous requests.\
With little interval(0.002s~) between requests, the lb does not seem to shut down.

## To Do
Socket 재활용, 기존의 서버에 Load Test를 했을 때, 더 나은 성능을 보임. LB와 차이점은 Socket의 재활용 여부, 따라서 Socket이 충돌하여 LB가 죽는 것으로 보임.\
Thread <-> Epoll 교체\
 