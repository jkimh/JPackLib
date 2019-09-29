# 개요
 - 게임 패치시 사용하는 런처에 필요한 Packing 라이브러리입니다. 간단하게 pack / unpack 이 가능하고, 패치시 필요한 pack 파일 머지 함수를 구현하였습니다.
 
# 테스트
- pack
  - bin 폴더 내 JPakConsole.exe 파일로 압축할 파일을 드래그 앤 드롭
- unpack
  - jpk 파일을 JPackConsole.exe 파일로 드래그앤 드롭
- merge
  - 콘솔창에서 JPakConsole.exe -merge a.jpk b.jpk 입력
  - a.jpk 파일에 b.jpk 내부 내용을 머지합니다.
