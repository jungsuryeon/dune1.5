24학번 정수련
1)~11)

구현하지 못한 것
보너스(사막 독수리 제외), 하코넨 유닛(A,F) 생성 및 공격, 건물이 파괴되면 장판이 나옴

애매한 부분
스파이스 주황색이 없어서 초록색으로 대체
바위 샌드윔은 통과할 수 없다고 해서 유닛은 통과하게 함
한 번에 수확하는 스파이스는 2~4인데 랜덤으로 처음은 가져오는데 1만 남았을 때는 그냥 가져옴

구현 방법
3) 샌드윔 구현 방법(함수 : POSITION  SANDWORM_find(POSITION worm_position))
  1. 유닛을 저장할 배열을 찾는다.
  2. 유닛의 개수를 찾는 함수(int find_H_positions(POSITION H_positions[])를 통해 유닛의 개수를 찾으며 배열에 저장한다.
  3. 맨해튼 거리 계산 함수(calculate_distance)를 통해 가장 가까운 유닛의 위치를 찾아서 반환하여 샌드윔의 도착지로 설정한다.

11) AI

    공통 사항
    - 플레이어와 ai는 공통으로 struct를 통해 기본 정보를 입력했다.
    - 구조체 배열에 넣어서 여러 개의 건물, 유닛이 생성돼도 구분이 가능하게 했다.
    - 생성할 수 있는 스파이스가 없다면 생성을 못 하게 한다.
    - P 제외 P 구조체 배열 안에 있는 위치를 찾고 그곳에 건물을 건설
    
    건물
      1. P(장판) 
         모든 건물의 기본이 되므로 15초마다 생성하게 했다.
         맵을 기준으로 오른쪽 1/2 크기에 랜덤하게 설치 했다.
         모든 공간이 비어 있을 때만 건설이 가능하게 하며 생성된다.
      2. A(투기장)
         투기장은 15초가 지나면 생성하게 했다.
         유닛을 계속 생성할 수 있으므로 투기장은 한 번만 생성하게 했다.
      3. F(공장)
         투기장은 40초가 지나면 생성하게 했다.
         유닛을 계속 생성할 수 있으므로 투기장은 한번만 생성하게 했다.
      4. D(숙소)
         스파이스 보유량이 20 이상일 때 생성한다.
      5. G(창고)
         인구수가 20 이상일 때 생성한다.

    유닛 H 하베스터
      생성(main에 있음)
         1. 샌드윔의 개수를 찾음
         2. H의 기본 위치를 지정하고 샌드윔이 4개가 넘을 때 생성 계속 생성되는 것은 아님
         3. 생성 이후 시간이 5초가 지나야지 다음 H를 생성할 수 있음
         4. H 생성하는 두 곳에 H가 있다면 생성할 수 없고 한곳에만 있다면 다른 한쪽에 생성 가능
         5. 생성되면 구조체 배열에 넣고 스파이스, 인구수 변경
      움직임(engine.c)
        1. 플레이어의 하베스터를 움직이는 것과 동일한 함수 사용
        (POSITION H_obj_next_position(OBJECT_SAMPLE* name),void H_obj_move(OBJECT_SAMPLE* name))
    
        2. 스파이스에 도착하면 스파이스를 2~4 랜덤하게 먹는다.
        3. void H_obj_move(OBJECT_SAMPLE* name) 이곳에서 스파이스 먹는 것을 조건해놨으며 스파이스를 먹으면 본진으로 목적지를 옮긴다.
        4. 다시 본진으로 돌아가면 가까운 스파이스를 찾는다. 스파이스를 찾는 것은 샌드윔이 하베스터를 찾는 것과 동일하다.
        (POSITION diff = psub((*name).dest, (*name).pos),diff.row == 0 && diff.column == 0)
        5. 만약 스파이스가 없다면 본진으로 간다.
        6. 샌드윔과 같은 H는 넘어가지 못하고 돌아서 가게 설정 했다.
        7. 만약 도착지에 H가 있다면 잠깐 기다린다.

    
