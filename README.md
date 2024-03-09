# ItemMap
### Plugin shows on the in-game map locations of all items, npcs, chests and other objects

## Functionality
- Show markers on map for items with ability to filter by plants, melee weapons, ranged weapons, armors, documents, spells (runes/scrolls), magic items, potions, food and miscellaneous
- Show markers on map for npcs with ability to filter by dead, angry, friendly, companions, hostile humans/monsters, traders and npcs that can be pickpocketed (only G2)
- Show markers on map for containers with ability to filter by locked (picklock/key owned by player/key not owned by player), open
- Show markers on map for items that are in containers
- Show markers on map for interactive objects that have a function
- Show list of all unique objects and their total amount
- Search for specific object by name or instance

## Screenshots
#### Help panel with controls
![Help panel with controls](https://i.imgur.com/Rmqu9EV.png)
#### All items
![All items](https://i.imgur.com/4BRyLyd.jpeg)
#### All npcs
![All npcs](https://i.imgur.com/nWuCfwL.jpeg)
#### Search npc by name
![Search npc by name](https://i.imgur.com/oUyfJ2D.jpeg)
#### Search item by name
![Search item by name](https://i.imgur.com/1ArKF5y.jpeg)
#### Search item by instance
![Search item by instance](https://i.imgur.com/2ITj4Bl.jpeg)

## Default ini settings
All settings besides colors, `ShowTradersNoCond`, `RememberSearchInput` and `ShowFilteredStaticColor` can be changed in-game while interacting with map and are saved after closing the map.
`ShowTradersNoCond`, `RememberSearchInput` and `ShowFilteredStaticColor` can be changed in Gothic.ini or game menu `Union & Plugins`.
Colors can be changed only in Gothic.ini
```
[ITEMMAP]
ShowSearchBar=1
; 
; Show search bar [default: 1]

ShowMarkers=1
; 
; Show markers on map [default: 1]

ShowList=0
; 
; Show list with items/npcs [default: 0]

ShowHelp=1
; 
; Show help panel with controls [default: 1]

IconSize=10
; 
; Size of the markers on map [default: 10]

ListWidth=25
; 
; Width of the list with items/npcs in % of the screen width [default: 25]

ShowTradersNoCond=0
; 
; Show traders without checking if player fulfill the requirements (traders that trader with certain guild, on certain hours or require to complete a quest beforehand) [default: 0]

RememberSearchInput=1
; 
; Remember search input when closing map or changing mode [default: 1]

PrevMode=0
; 
; Previously used mode [default: 0 - Items]

PrevFilterItems=0
; 
; Previously used filter for items [default: 0 - Plants]

PrevFilterNpcs=8
; 
; Previously used filter for npcs [default: 9(G1)/10(G2) - All]

ColorItemPlants=#00FF00
; 
; Marker color for plants [default: #00FF00]

ColorItemMelee=#FF0000
; 
; Marker color for melee weapons [default: #FF0000]

ColorItemRanged=#FF0000
; 
; Marker color for ranged weapons [default: #FF0000]

ColorItemArmors=#800080
; 
; Marker color for armors [default: #800080]

ColorItemDocs=#FFFF7F
; 
; Marker color for documents [default: #FFFF7F]

ColorItemSpells=#0080FF
; 
; Marker color for spells [default: #0080FF]

ColorItemMagicItems=#FF00FF
; 
; Marker color for magic items [default: #FF00FF]

ColorItemPotions=#00FFFF
; 
; Marker color for potions [default: #00FFFF]

ColorItemFood=#FF8000
; 
; Marker color for food [default: #FF8000]

ColorItemNone=#808080
; 
; Marker color for misc items [default: #808080]

ColorNpcDead=#000000
; 
; Marker color for dead npcs [default: #000000]

ColorNpcAngry=#FF8000
; 
; Marker color for angry npcs [default: #FF8000]

ColorNpcFriendly=#00FF00
; 
; Marker color for friendly npcs [default: #00FF00]

ColorNpcParty=#AFFFAF
; 
; Marker color for companions [default: #AFFFAF]

ColorNpcHostileHuman=#C800C8
; 
; Marker color for hostile humans [default: #C800C8]

ColorNpcHostileMonster=#FF0000
; 
; Marker color for hostile monsters [default: #FF0000]

ColorNpcTrader=#FFFF80
; 
; Marker color for traders [default: #FFFF80]

ColorNpcPickpocket=#80AFFF
; 
; Marker color for npcs that can be pickpocketed [default: #80AFFF]

ShowFilteredStaticColor=0
; 
; Use static color for all markers when currently active filter is other than ALL [default: 0]

ColorStaticFilter=#FFFFFF
; 
; Marker color used when "ShowFilteredStaticColor" is active [default: #FFFFFF]
```