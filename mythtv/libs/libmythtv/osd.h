#ifndef OSD_H
#define OSD_H

// MythTV
#include "mythtvexp.h"
#include "programtypes.h"
#include "mythscreentype.h"
#include "mythtypes.h"
#include "mythplayerstate.h"
#include "mythmediaoverlay.h"

// Screen names are prepended with alphanumerics to force the correct ordering
// when displayed. This is slightly complicated by the default windows
// (e.g. osd_window) whose names are hard coded into existing themes.

// menu dialogs should always be on top
#define OSD_DLG_VIDEOEXIT "xx_OSD_VIDEO_EXIT"
#define OSD_DLG_MENU      "xx_OSD_MENU"
#define OSD_DLG_SLEEP     "xx_OSD_SLEEP"
#define OSD_DLG_IDLE      "xx_OSD_IDLE"
#define OSD_DLG_INFO      "xx_OSD_INFO"
#define OSD_DLG_EDITING   "xx_OSD_EDITING"
#define OSD_DLG_ASKALLOW  "xx_OSD_ASKALLOW"
#define OSD_DLG_EDITOR    "xx_OSD_EDITOR"
#define OSD_DLG_CUTPOINT  "xx_OSD_CUTPOINT"
#define OSD_DLG_DELETE    "xx_OSD_DELETE"
#define OSD_DLG_NAVIGATE  "xx_OSD_NAVIGATE"
#define OSD_DLG_CONFIRM   "mythconfirmpopup"

#define OSD_WIN_MESSAGE  "osd_message"
#define OSD_WIN_INPUT    "osd_input"
#define OSD_WIN_PROGINFO "program_info"
#define OSD_WIN_STATUS   "osd_status"
#define OSD_WIN_DEBUG    "osd_debug"
#define OSD_WIN_BROWSE   "browse_info"
#define OSD_WIN_PROGEDIT "osd_program_editor"

#define kOSDFadeTime 1000

class TV;
class MythMainWindow;
class MythPlayerUI;


enum OSDFunctionalType
{
    kOSDFunctionalType_Default = 0,
    kOSDFunctionalType_PictureAdjust,
    kOSDFunctionalType_SmartForward,
    kOSDFunctionalType_TimeStretchAdjust,
    kOSDFunctionalType_AudioSyncAdjust,
    kOSDFunctionalType_SubtitleZoomAdjust,
    kOSDFunctionalType_SubtitleDelayAdjust
};

enum OSDTimeout
{
    kOSDTimeout_Ignore = -1, // Don't update existing timeout
    kOSDTimeout_None   = 0,  // Don't timeout
    kOSDTimeout_Short  = 1,
    kOSDTimeout_Med    = 2,
    kOSDTimeout_Long   = 3,
};

class MythOSDDialogData
{
  public:
    class MythOSDDialogButton
    {
      public:
        QString  m_text    { };
        QVariant m_data    { };
        bool     m_menu    { false };
        bool     m_current { false };
    };

    class MythOSDBackButton
    {
      public:
        QString  m_text { };
        QVariant m_data { 0 };
        bool     m_exit { false };
    };

    QString m_dialogName;
    QString m_message { };
    int     m_timeout { kOSDTimeout_None };
    std::vector<MythOSDDialogButton> m_buttons { };
    MythOSDBackButton m_back { };
};

Q_DECLARE_METATYPE(MythOSDDialogData)

class OSD : public MythMediaOverlay
{
    Q_OBJECT

  signals:
    void HideOSD(OSDFunctionalType Type);

  public slots:
    void SetText(const QString& Window, const InfoMap& Map, OSDTimeout Timeout);
    void DialogQuit();
    void HideAll(bool KeepSubs = true, MythScreenType* Except = nullptr, bool DropNotification = false);

  protected slots:
    void ShowDialog(const MythOSDDialogData& Data);
    void IsOSDVisible(bool& Visible);

  public:
    OSD(MythMainWindow* MainWindow, TV* Tv, MythPlayerUI* Player, MythPainter* Painter);
   ~OSD() override;

    bool Init(QRect Rect, float FontAspect) override;
    void HideWindow(const QString &Window) override;
    void SetFunctionalWindow(const QString &Window, enum OSDFunctionalType Type);

    void SetExpiry(const QString &Window, enum OSDTimeout Timeout, int CustomTimeout = 0);
    void ResetWindow(const QString &Window);
    void Draw(QRect Rect);

    void SetValues(const QString &Window, const QHash<QString,int> &Map, OSDTimeout Timeout);
    void SetValues(const QString &Window, const QHash<QString,float> &Map, OSDTimeout Timeout);
    void SetRegions(const QString &Window, frm_dir_map_t &Map, long long Total);
    void SetGraph(const QString &Window, const QString &Graph, int64_t Timecode);
    bool IsWindowVisible(const QString &Window);

    bool DialogVisible(const QString& Window = QString());
    bool DialogHandleKeypress(QKeyEvent *Event);
    bool DialogHandleGesture(MythGestureEvent *Event);
    void DialogGetText(InfoMap &Map);

  private:
    void PositionWindow(MythScreenType* Window);
    void RemoveWindow(const QString& Window);
    void DialogShow(const QString& Window, const QString& Text = "", int UpdateFor = 0);
    void DialogAddButton(const QString& Text, QVariant Data, bool Menu = false, bool Current = false);
    void DialogBack(const QString& Text = "", const QVariant& Data = 0, bool Exit = false);
    void TearDown() override;
    void LoadWindows();
    void CheckExpiry();
    void SetExpiryPriv(const QString &Window, enum OSDTimeout Timeout, int CustomTimeout);

  private:
    int             m_fadeTime          { kOSDFadeTime };
    MythScreenType* m_dialog            { nullptr };
    QString         m_pulsedDialogText  { };
    QDateTime       m_nextPulseUpdate   { };
    std::array<int,4> m_timeouts        { -1, 3000, 5000, 13000 };
    enum OSDFunctionalType m_functionalType { kOSDFunctionalType_Default };
    QString                m_functionalWindow { };
    QHash<MythScreenType*, QDateTime> m_expireTimes { };
};

#endif
