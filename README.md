# Arduino Light Data Transmit

한국디지털미디어고등학교 시스템프로그래밍 수행평가, 두개의 아두이노에서 회로 연결 없이 데이터 전송하기 (단방향)

"회로 연결이 없어야 함"을 제외하고는 그 어떤 제약사항도 없음. 우리 팀은 두 켤레의 LED와 조도센서를 활용함. 다른 조는 서보모터 + 가변저항을 쓰기도 하더라고요.

한번에 신호를 두 개 보내기 위해 빛채널 두 켤레를 사용함. 1채널은 "지금 데이터가 전송중인가"를 알려주고, 2채널은 실질적으로 데이터가 전송된다. 따라서 데이터가 전송중인 동안 1채널의 LED는 항상 켜져있고, 2채널은 깜빡이게 된다.

## 송신부

2채널에 들어갈 Payload는 다음의 공식으로 도출하였다.

```python
# Payload 생성 함수
# 알파벳 글자를 10진수 1 ~ 26 범위의 5bit로 변환하고 모두 이어붙힘
def createPayload(string data):
    int bits[]
    # 각 글자마다 반복
    for char of data:
        # A가 1로, Z가 26으로 매핑
        int ascii = char.toASCII() - 64
        # 1 ~ 26 범위를 ASCII로 변환
        # 최대값이 26이기에 5bits로 표현 가능
        binaries = ascii.toBinary()[0:5]

        bits.append(binaries)
```

채널 2에서는 생성한 Payload의 값을 앞에서부터 100ms에 한 번씩 전송한다. 보내야할 digit이 1이면 LED를 켜고, 0이면 끔. Payload를 보내는 동안 채널 1은 항상 켜져있음.

## 수신부

채널 1이 켜져 있는 동안만 loop 로직을 실행하도록 하였다. 