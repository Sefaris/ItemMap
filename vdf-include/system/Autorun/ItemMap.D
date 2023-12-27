META
{
  Parser = Menu;
  After = zUnionMenu.d;
  Namespace = ItemMap;
};

// Namespace = ItemMap
// Important !!!
// Supporting Unicode symbols.
// File encoding: UTF-8 (without BOM).

// ------ Constants ------
const int Start_PY  = 1400;
const int Title_PY  = 450;
const int Menu_DY   = 550;
// Text
const int Text_PX   = 1500;  // Position at left side
const int Text_SX   = 8000; // Size X
const int Text_SY   = 750;  // Size Y
const int Text_DY   = 120;  // Delta alignment
// Slider
const int Slider_PX = 5500; // Position at left side
const int Slider_SX = 1500; // Size X
const int Slider_SY = 600;  // Size Y
const int Slider_DY = 0;    // Delta alignment
// Choice
const int Choice_PX = 5500; // Position at left side
const int Choice_SX = 1500; // Size X
const int Choice_SY = 350;  // Size Y
const int Choice_DY = 120;  // Delta alignment

const string MenuBackPic   = "MENU_INGAME.TGA";
const string ItemBackPic   = "";
const string ChoiceBackPic = "MENU_CHOICE_BACK.TGA";
const string SliderBackPic = "MENU_SLIDER_BACK.TGA";
const string SliderPosPic  = "MENU_SLIDER_POS.TGA";
const string FontSmall     = "font_old_10_white.tga";
const string FontBig       = "font_old_20_white.tga";

var int CurrentMenuItem_PY;

// ------ Prototypes ------
func void InitializeBackPicturesAndFonts()
{
  MenuBackPic   = MENU_BACK_PIC;
  ItemBackPic   = MENU_ITEM_BACK_PIC;
  ChoiceBackPic = MENU_CHOICE_BACK_PIC;
  SliderBackPic = MENU_SLIDER_BACK_PIC;
  SliderPosPic  = MENU_SLIDER_POS_PIC;
  FontSmall     = MENU_FONT_SMALL;
  FontBig       = MENU_FONT_DEFAULT;
};

prototype C_EMPTY_MENU_DEF(C_MENU)
{
  InitializeBackPicturesAndFonts();
  C_MENU_DEF();
  backpic    = MenuBackPic;
  items[0]   = "";
  items[100] = "Union_menuitem_back";
  flags      = flags | MENU_SHOW_INFO;
};

instance C_MENU_ITEM_TEXT_BASE(C_MENU_ITEM_DEF)
{
  backpic        = ItemBackPic;
  posx           = Text_PX;
  posy           = Start_PY;
  dimx           = Text_SX;
  dimy           = Text_SY;
  flags          = flags | IT_EFFECTS_NEXT;
  onselaction[0] = SEL_ACTION_UNDEF;
};

instance C_MENUITEM_CHOICE_BASE(C_MENU_ITEM_DEF)
{
  backpic  = ChoiceBackPic;
  type     = MENU_ITEM_CHOICEBOX;
  fontname = FontSmall;
  posx     = Choice_PX;
  posy     = Start_PY + Choice_DY;
  dimx     = Choice_SX;
  dimy     = Choice_SY;
  flags    = flags & ~IT_SELECTABLE;
  flags    = flags | IT_TXT_CENTER;
};

instance C_MENUITEM_SLIDER_BASE(C_MENU_ITEM_DEF)
{
  backpic       = SliderBackPic;
  type          = MENU_ITEM_SLIDER;
  fontname      = FontSmall;
  posx          = Slider_PX;
  posy          = Start_PY + Slider_DY;
  dimx          = Slider_SX;
  dimy          = Slider_SY;
  flags         = flags & ~IT_SELECTABLE;
  flags         = flags | IT_TXT_CENTER;
  userstring[0] = SliderPosPic;
};

instance MenuItem_Opt_Headline(C_MENU_ITEM_DEF)
{
  type    = MENU_ITEM_TEXT;
  posx    = 0;
  posy    = Title_PY;
  dimx    = 8100;
  flags   = flags & ~IT_SELECTABLE;
  flags   = flags | IT_TXT_CENTER;
  text[0] = Str_GetLocalizedString(
    "ItemMap НАСТРОЙКИ",
    "ItemMap SETTINGS",
    "ItemMap EINSTELLUNGEN",
    "USTAWIENIA ItemMap"
  );
};

func int Act_OpenWebLink()
{
  Open_Link("https://github.com/Sefaris/ItemMap");
  return 0;
};

instance MenuItem_Opt_Open_Link(C_MENU_ITEM_DEF)
{
  C_MENU_ITEM_TEXT_BASE();
  posy += MENU_DY * 8;

  posx             = 64;
  onselaction[0]   = SEL_ACTION_UNDEF;
  oneventaction[1] = Act_OpenWebLink;
  flags            = flags | IT_TXT_CENTER;
  text[0]          = Str_GetLocalizedString(
    "Открыть страницу проекта",
    "Open project page",
    "Projektseite öffnen",
    "Otwórz stronę projektu"
  );

  text[1]          = "https://github.com/Sefaris/ItemMap";
};

// ------ Menu ------
instance MenuItem_Union_Auto_ItemMap(C_MENU_ITEM_UNION_DEF)
{
  text[0]          = "ItemMap";
  text[1] = Str_GetLocalizedString(
    "Наѝтройте параметры ItemMap",
    "Configure ItemMap settings",
    "ItemMap-Einstellungen konfigurieren",
    "Zmień ustawienia ItemMap"
  );
  onselaction[0]   = SEL_ACTION_STARTMENU;
  onselaction_s[0] = "ItemMap:Menu_Opt_ItemMap";
};

instance Menu_Opt_ItemMap(C_EMPTY_MENU_DEF)
{
  Menu_SearchItems("ItemMap:MENUITEM_OPT_ItemMap_*");
};

instance MenuItem_Opt_ItemMap_Headline(C_MENU_ITEM)
{
  MenuItem_Opt_Headline();
};

instance MenuItem_Opt_ItemMap_RememberSearchInput(C_MENU_ITEM)
{
  CurrentMenuItem_PY = CurrentMenuItem_PY + 1;
  C_MENU_ITEM_TEXT_BASE();
  fontname = FontSmall;
  posy += Menu_DY * CurrentMenuItem_PY + Text_DY;
  
  text[0] = Str_GetLocalizedString(
    "Запомнить текст в поиске",
    "Remember search input",
    "Sucheingabe merken",
    "Zapamiętaj frazę wyszukiwania"
  );
  text[1] = Str_GetLocalizedString(
    "Запомнить текст в поиске при закрытии карты или смене мода",
    "Remember search input when closing map or changing mode",
    "Sucheingabe merken beim Schließen der Karte oder beim Wechsel des Modus",
    "Zapamiętaj frazę wyszukiwania przy zamykaniu mapy lub zmianie trybu"
  );
};

instance MenuItem_Opt_ItemMap_RememberSearchInput_Choice(C_MENU_ITEM_DEF)
{
  C_MENUITEM_CHOICE_BASE();
  posy += Menu_DY * CurrentMenuItem_PY;
  
  onchgsetoption        = "RememberSearchInput";
  onchgsetoptionsection = "ItemMap";
  text[0]               = Str_GetLocalizedString(
    "Выкл.|Вкл.",
    "Off|On",
    "Aus|Ein",
    "Wył.|Wł."
  );
};

instance MenuItem_Opt_ItemMap_ShowTradersNoCond(C_MENU_ITEM)
{
  CurrentMenuItem_PY = CurrentMenuItem_PY + 1;
  C_MENU_ITEM_TEXT_BASE();
  fontname = FontSmall;
  posy += Menu_DY * CurrentMenuItem_PY + Text_DY;
  
  text[0] = Str_GetLocalizedString(
    "Показать всех торговцев",
    "Show all traders",
    "Zeige alle Händler",
    "Pokaż wszystkich handlarzy"
  );
  text[1] = Str_GetLocalizedString(
    "Показать всех торговцев, не проверяя, соответствует ли игрок требованиям (предыдущий квест/разговор)",
    "Show all traders without checking if player meets the requirements (previous quest/conversation)",
    "Alle Händler anzeigen, ohne zu prüfen, ob der Spieler die Anforderungen erfüllt (vorherige Quest/Gespräch)",
    "Pokaż wszystkich handlarzy bez sprawdzania czy gracz spełnia wymagania (uprzedni quest/rozmowa)"
  );
};

instance MenuItem_Opt_ItemMap_ShowTradersNoCond_Choice(C_MENU_ITEM_DEF)
{
  C_MENUITEM_CHOICE_BASE();
  posy += Menu_DY * CurrentMenuItem_PY;
  
  onchgsetoption        = "ShowTradersNoCond";
  onchgsetoptionsection = "ItemMap";
  text[0]               = Str_GetLocalizedString(
    "Выкл.|Вкл.",
    "Off|On",
    "Aus|Ein",
    "Wył.|Wł."
  );
};

instance MenuItem_Opt_ItemMap_ShowFilteredStaticColor(C_MENU_ITEM)
{
  CurrentMenuItem_PY = CurrentMenuItem_PY + 1;
  C_MENU_ITEM_TEXT_BASE();
  fontname = FontSmall;
  posy += Menu_DY * CurrentMenuItem_PY + Text_DY;
  
  text[0] = Str_GetLocalizedString(
    "Статический цвет для фильтров",
    "Static color for filters",
    "Statische Farbe für Filter",
    "Statyczny kolor dla filtrów"
  );
  text[1] = Str_GetLocalizedString(
    "Использовать статический цвет для всех маркеров, если текущий активный фильтр отличается от ВСЕ.",
    "Use static color for all markers if currently active filter is other than ALL",
    "Statische Farbe für alle Markierungen verwenden, wenn der derzeit aktive Filter ein anderer als ALLE ist",
    "Użyj statycznego koloru jeśli aktualnie wybrany filtr jest inny niż ALL"
  );
};

instance MenuItem_Opt_ItemMap_ShowFilteredStaticColor_Choice(C_MENU_ITEM_DEF)
{
  C_MENUITEM_CHOICE_BASE();
  posy += Menu_DY * CurrentMenuItem_PY;
  
  onchgsetoption        = "ShowFilteredStaticColor";
  onchgsetoptionsection = "ItemMap";
  text[0]               = Str_GetLocalizedString(
    "Выкл.|Вкл.",
    "Off|On",
    "Aus|Ein",
    "Wył.|Wł."
  );
};

instance MenuItem_Opt_ItemMap_Open_Link(C_MENU_ITEM)
{
  MenuItem_Opt_Open_Link();
};