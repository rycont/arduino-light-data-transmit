# Arduino Light Data Transmit

한국디지털미디어고등학교 시스템프로그래밍 수행평가, 두개의 아두이노에서 회로 연결 없이 데이터 전송하기 (단방향)

"회로 연결이 없어야 함"을 제외하고는 그 어떤 제약사항도 없음. 우리 팀은 두 켤레의 LED와 조도센서를 활용함. 다른 조는 서보모터 + 가변저항을 쓰기도 하더라고요.

한번에 신호를 두 개 보내기 위해 빛채널 두 켤레를 사용함. 1채널은 "지금 데이터가 전송중인가"를 알려주고, 2채널은 실질적으로 데이터가 전송된다. 따라서 데이터가 전송중인 동안 1채널의 LED는 항상 켜져있고, 2채널은 깜빡이게 된다.

## 송신부

2채널에 들어갈 Payload는 다음의 공식으로 도출하였다.

```python
# Payload 생성 함수
# 알파벳 글자를 10진수 1 ~ 26 범위의 5bit로 변환하고 모두 이어붙힘
def createPayload(string data) {
    int bits[]
    # 각 글자마다 반복
    for(char of data) {
        # A가 1로, Z가 26으로 매핑
        int ascii = char.toASCII() - 64
        # 1 ~ 26 범위를 ASCII로 변환
        # 최대값이 26이기에 5bits로 표현 가능
        binaries = ascii.toBinary()[0:5]

        bits.extend(binaries)
    }
}
```

채널 2에서는 생성한 Payload의 값을 앞에서부터 100ms에 한 번씩 전송한다. 보내야할 digit이 1이면 LED를 켜고, 0이면 끔. Payload를 보내는 동안 채널 1은 항상 켜져있음.

## 수신부

데이터가 송신중인 동안만 loop 로직을 실행하도록 하였다. 채널 1이 켜져 있는 동안 100ms마다 채널 2의 값을 5번 읽어와서 10진수로 변환하고, 채널 1이 꺼질 때 까지 계속 반복한다.

```python
# Payload 수신 함수

def loop() {
    # 1채널이 꺼져있다면 무시
    if(!channel[1].getData()) return

    int datas[] = []

    do {
        int digits[] = [ 1 ] * 5

        # 5 Digit만큼 100ms씩 끊어서 읽어오기
        # (0 | 1)[] 형식으로 저장됨

        for(index in digits) {
            digits[index] = channel[2].getData()
            delay(100)
        }

        # 받아온 digits를 Int로 변환
        int data = binaryArrayToInt(digits)

        # 출력 형식은 알파벳
        print((data + 64).toASCIIString())

        datas.push(data)
        
    # 1채널이 켜져있는 동안 반복
    } while(channel[1].getData())

    print(datas)
}    
```

### 문제해결

> #### 1. 조도센서의 역치가 일정하지 않음

아이패드 케이스로 덮어서 주변 환경을 어둡게 만들어 해결

> #### 2. 조도센서의 반응속도가 늦음

![](./Frame%201.png)

데이터 전송 사이클인 100ms마다 LED를 점멸함과 동시에 조도센서에서 값을 받아오기 때문에, 미처 LED의 밝기가 조도센서에 반영되기 전에 값이 측정되어 이전의 점멸 상태가 유지됨.

-  해결 방법: 조도센서 수집 시기를 `사이클 시작 + 50ms`로 수정하여 제대로 반영되었을 때 측정함

```python
for(index in digits) {
    delay(50)
    digits[index] = channel[2].getData()
    delay(50)
}
```

> #### 3. 외부 밝기 노이즈 영향이 커서 역치 필터가 제대로 작동하지 않음

간이로 암실환경을 만들었기 때문에 외부 노이즈를 확실하게 제거하기 힘들었고, 바람 / 진동에 의해 덮개가 흔들리거나 교실에 조명이 추가로 켜지는 등 외부 요인에 의해 간헐적으로 역치를 넘어서는 노이즈가 유입되었다.

- 해결 방법: 한 사이클 안에 20ms 단위로 5번 측정하고, 평균값을 사용해서 노이즈 오차를 억제하였다. 최초 측정값은 사이클 시작 10ms 이후에 수집한다.

```python
for(index in digits) {
    delay(10)
    digits[index] = channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(10)
}
```

> #### 4. 아두이노의 계산시간에 의한 싱크 사이클 틀어짐

`delay` 함수로 매 100ms씩 사이클을 순환하였으나, 전송이 진행될수록 사이클이 뒤로 밀리는 문제를 발견하였다. `delay` 함수의 오차 + 사이클마다의 자체 계산시간에 의해 순환이 지연되는 것으로 추측했다.

- 해결 방법: 사이클을 100ms가 조금 안되게 줄임

```python
for(index in digits) {
    delay(10)
    digits[index] = channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(20)
    digits[index] += channel[2].getData()
    delay(9)
}
```
