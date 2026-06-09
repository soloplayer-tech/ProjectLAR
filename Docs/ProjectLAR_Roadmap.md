# ProjectLAR Roadmap

Last updated: 2026-06-08

## Direction

ProjectLAR는 로스트아크식 쿼터뷰 액션 RPG 전투 구조를 목표로 한다.
현재는 플레이어 전투, 스킬 슬롯, 쿨다운 UI, 아이덴티티, Damageable 테스트, 상호작용, Skill DataAsset 기반 구조가 일부 구현되어 있다.

보스 시스템은 이 로드맵의 직접 작업 범위에서 제외한다.

## Current Foundation

- 우클릭 이동
- 마우스 방향 기본 공격
- 블링크형 대시
- Q/W/E/R/A/S/D/F/V 액션 스킬 슬롯
- 스킬창 드래그 앤 드롭
- 슬롯 간 스킬 이동 및 제거
- 중복 스킬 장착 방지
- 스킬 슬롯 SaveGame 저장/로드
- 액션 슬롯 쿨다운 UI
- HP / MP / 아이덴티티 UI
- Z 아이덴티티 발동
- 캐스팅 일부 구현 및 이동/대시 캔슬
- Damageable 인터페이스 기반 데미지 테스트
- G키 상호작용 Actor
- `LPlayerSkillDataAsset` / `LPlayerSkillDatabase` 기반 스킬 데이터 구조

## Priority 1: Skill Data Pipeline

목표: 스킬 밸런스 값의 기준점을 C++ 하드코딩에서 DataAsset/Database로 옮긴다.

- `LPlayerSkillDatabase` 에셋 생성
- Meteor / IceLance / Thunder / Wind 개별 `LPlayerSkillDataAsset` 생성
- Database에 개별 SkillDataAsset 등록
- `BP_PlayerCharacter`의 `SkillDatabase`에 Database 연결
- DataAsset 값이 마나/쿨다운/캐스팅/스킬락에 실제 반영되는지 검증
- 중복 SkillID 및 누락 SkillID 검증 로그 확인

## Priority 2: Basic Attack Montage Prototype

목표: 모든 스킬을 한 번에 바꾸지 않고, 기본 공격 하나로 AnimMontage + Notify 기반 판정 구조를 검증한다.

- BasicAttack AnimMontage 준비
- 입력 시 마우스 방향 회전 후 Montage 재생
- 데미지 판정을 AnimNotify 또는 AnimNotifyState로 이동
- 기존 타이머 기반 BasicAttack 종료 로직을 Montage 종료/Notify 흐름과 비교
- 기본 공격 VFX/SFX 타이밍을 Notify로 분리

## Priority 3: Skill Montage Conversion

목표: 검증된 BasicAttack 구조를 일반 스킬로 확장한다.

우선 순서:

1. Wind
2. IceLance
3. Thunder
4. Meteor

작업 내용:

- 스킬별 Montage 연결
- 시전/발사/타격 타이밍을 Notify로 분리
- Active Hit 구간을 NotifyState로 관리
- SkillLockDuration과 Montage Recovery 구간의 역할 정리

## Priority 4: Awakening Slot

목표: V 슬롯을 일반 스킬과 분리된 각성기 슬롯으로 정리한다.

- Q/W/E/R/A/S/D/F: 일반 스킬 슬롯
- V: Awakening Skill Slot
- V 전용 쿨다운
- V 전용 사용 가능 조건
- V 전용 UI 표현

## Priority 5: MeteorRain Awakening Skill

목표: MeteorRain을 V 각성기로 구현한다.

- 지정 위치 또는 전방 위치에 마법진 생성
- 여러 개의 Meteor Actor 순차 생성
- 마지막 대폭발 판정
- 긴 쿨다운
- 강한 데미지
- 전용 Montage / Niagara / Sound

## Priority 6: Additional Skill Types

목표: 서로 다른 조작감을 가진 스킬 타입을 2~3개 추가한다.

후보:

- 전방 검기형 스킬
- 원형 폭발형 스킬
- 설치형 지속 장판 스킬

## Priority 7: Lightweight Growth Loop

목표: 로스트아크식 성장 요소를 매우 단순한 형태로 구현한다.

- InventoryComponent
- Gold
- Enhancement Stone
- Weapon only equipment
- Weapon enhance level
- Weapon attack power added to skill damage
- SaveGame 확장

## Priority 8: Enhancement Station

목표: 기존 상호작용 구조를 활용해 무기 강화 장소를 만든다.

- 강화 Actor
- G키 상호작용
- 강화 UI
- 골드/강화석 소모
- 무기 강화 레벨 +1
- 초기 강화는 100% 성공

## Notes

- 전투 손맛이 먼저다. 인벤토리/강화는 전투 코어 이후로 미룬다.
- 새 스킬은 가능한 한 DataAsset/Database 기반으로 추가한다.
- 공격 판정은 최종적으로 AnimNotify 또는 AnimNotifyState로 이동한다.
- BasicAttack은 현재 SkillDataAsset 비용 소모 흐름에 연결되어 있지 않다.
- 보스 시스템은 별도 파트로 분리한다.
