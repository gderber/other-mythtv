#ifndef MYTHEDID_H
#define MYTHEDID_H

// Qt
#include <QSize>
#include <QStringList>

// MythTV
#include "mythuiexp.h"

// Std
#include <utility>
#include <array>

using PrimarySpace = std::array<std::array<float,2>,3>;
using WhiteSpace = std::array<float,2>;

class MUI_PUBLIC MythEDID
{
  public:
    // This structure matches VideoColourSpace::ColourPrimaries
    // TODO move ColourPrimaries into MythDisplay
    struct Primaries
    {
        PrimarySpace primaries;
        WhiteSpace   whitepoint;
    };

    enum HDREOTF
    {
        SDR     = 1 << 0,
        HDRTrad = 1 << 1,
        HDR10   = 1 << 2,
        HLG     = 1 << 3
    };

    enum HDRMetadaType
    {
        Static1 = 1 << 0
    };

    MythEDID() = default;
    explicit MythEDID(QByteArray  Data);
    MythEDID(const char* Data, int Length);

    bool        Valid             () const;
    QStringList SerialNumbers     () const;
    QSize       DisplaySize       () const;
    double      DisplayAspect     () const;
    uint16_t    PhysicalAddress   () const;
    float       Gamma             () const;
    bool        IsHDMI            () const;
    bool        IsSRGB            () const;
    bool        IsLikeSRGB        () const;
    Primaries   ColourPrimaries   () const;
    int         AudioLatency      (bool Interlaced) const;
    int         VideoLatency      (bool Interlaced) const;
    void        Debug             () const;
    std::pair<int,int> GetHDRSupport() const;
    static QString EOTFToString   (int EOTF);
    static QStringList EOTFToStrings(int EOTF);

  private:
    void        Parse             ();
    bool        ParseBaseBlock    (const quint8* Data);
    bool        ParseCTA861       (const quint8* Data, uint Offset);
    bool        ParseCTABlock     (const quint8* Data, uint Offset);
    bool        ParseVSDB         (const quint8* Data, uint Offset, uint Length);
    bool        ParseExtended     (const quint8* Data, uint Offset, uint Length);

    bool        m_valid           { false };
    QByteArray  m_data            { };
    uint        m_size            { 0 };
    quint8      m_minorVersion    { 0 };
    QSize       m_displaySize     { };    // N.B. Either size or aspect are valid
    double      m_displayAspect   { 0.0 };
    QStringList m_serialNumbers   { };
    float       m_gamma           { 0.0F }; // Invalid
    bool        m_sRGB            { false };
    bool        m_likeSRGB        { false }; // Temporary until Colourspace handling in libmythui
    Primaries   m_primaries       { {{{0.0F, 0.0F}, {0.0F, 0.0F}, {0.0F, 0.0F}}}, {0.0F, 0.0F} };
    bool        m_isHDMI          { false };
    uint16_t    m_physicalAddress { 0 };
    uint8_t     m_deepColor       { 0 };
    bool        m_latencies       { false };
    bool        m_interLatencies  { false };
    std::array<int,2> m_audioLatency { 0 };
    std::array<int,2> m_videoLatency { 0 };
    uint8_t     m_deepYUV         { 0 };
    int         m_vrrMin          { 0 };
    int         m_vrrMax          { 0 };
    int         m_hdrSupport      { 0 };
    int         m_hdrMetaTypes    { 0 };
    float       m_maxLuminance    { 0.0F };
    float       m_maxAvgLuminance { 0.0F };
    float       m_minLuminance    { 0.0F };
};

#endif
