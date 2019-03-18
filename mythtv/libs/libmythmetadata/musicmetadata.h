#ifndef MUSICMETADATA_H_
#define MUSICMETADATA_H_

// C/C++
#include <cstdint>

// qt
#include <QStringList>
#include <QMap>
#include <QDateTime>
#include <QImage>
#include <QMetaType>
#include <QCoreApplication>

// mythtv
#include "mythtypes.h"
#include "mythmetaexp.h"
#include "mthread.h"
#include "mythcorecontext.h"

class AllMusic;
class AlbumArtImages;
class LyricsData;
class MetaIO;

enum ImageType
{
    IT_UNKNOWN = 0,
    IT_FRONTCOVER,
    IT_BACKCOVER,
    IT_CD,
    IT_INLAY,
    IT_ARTIST,
    IT_LAST
};

class META_PUBLIC AlbumArtImage
{
  public:
    AlbumArtImage(void) :
            m_filename(""), m_hostname(""), m_description("") {}
    AlbumArtImage(AlbumArtImage *image) :
            m_id(image->m_id), m_filename(image->m_filename),
            m_hostname(image->m_hostname), m_imageType(image->m_imageType),
            m_description(image->m_description), m_embedded(image->m_embedded) {}
     int       m_id          {0};
     QString   m_filename;
     QString   m_hostname;
     ImageType m_imageType   {IT_UNKNOWN};
     QString   m_description;
     bool      m_embedded    {false};
};

typedef QList<AlbumArtImage*> AlbumArtList;

enum RepoType
{
    RT_Database = 0,
    RT_CD       = 1,
    RT_Radio    = 2
};

#define METADATA_BITS_FOR_REPO 8
#define METADATA_REPO_SHIFT 24
#define METADATA_REPO_MASK 0xff000000
#define METADATA_ID_MASK 0x00ffffff

#define ID_TO_ID(x) x & METADATA_ID_MASK;
#define ID_TO_REPO(x)  x >> METADATA_REPO_SHIFT

#define METADATA_INVALID_FILENAME "**NOT FOUND**"

#define STREAMUPDATEURL "https://services.mythtv.org/music/data/?data=streams"
#define STREAMURLCOUNT 5

typedef QString UrlList[STREAMURLCOUNT];

class META_PUBLIC MusicMetadata
{
    Q_DECLARE_TR_FUNCTIONS(MusicMetadata);

  public:

    typedef uint32_t IdType;

    MusicMetadata(QString lfilename = "", QString lartist = "", QString lcompilation_artist = "",
             QString lalbum = "", QString ltitle = "", QString lgenre = "",
             int lyear = 0, int ltracknum = 0, int llength = 0, int lid = 0,
             int lrating = 0, int lplaycount = 0, QDateTime llastplay = QDateTime(),
             QDateTime ldateadded = QDateTime(), bool lcompilation = false, QString lformat = "")
                : m_artist(lartist),
                   m_compilation_artist(lcompilation_artist),
                   m_album(lalbum),
                   m_title(ltitle),
                   m_genre(lgenre),
                   m_format(lformat),
                   m_year(lyear),
                   m_tracknum(ltracknum),
                   m_length(llength),
                   m_rating(lrating),
                   m_lastplay(llastplay),
                   m_dateadded(ldateadded),
                   m_playcount(lplaycount),
                   m_compilation(lcompilation),
                   m_id(lid),
                   m_filename(lfilename)
    {
        checkEmptyFields();
        ensureSortFields();
    }

    MusicMetadata(int lid, QString lbroadcaster, QString lchannel, QString ldescription, UrlList lurls, QString llogourl,
             QString lgenre, QString lmetaformat, QString lcountry, QString llanguage, QString lformat);

    ~MusicMetadata();

    MusicMetadata(const MusicMetadata &other)
    {
        *this = other;
         m_changed = false;
    }

    MusicMetadata& operator=(const MusicMetadata &rhs);

    QString Artist() const { return m_artist; }
    QString ArtistSort() const { return m_artist_sort; }
    void setArtist(const QString &lartist,
                   const QString &lartist_sort = nullptr)
    {
        m_artist = lartist;
        m_artist_sort = lartist_sort;
        m_formattedartist.clear(); m_formattedtitle.clear();
        ensureSortFields();
    }

    QString CompilationArtist() const { return m_compilation_artist; }
    QString CompilationArtistSort() const { return m_compilation_artist_sort; }
    void setCompilationArtist(const QString &lcompilation_artist,
                              const QString &lcompilation_artist_sort = nullptr)
    {
        m_compilation_artist = lcompilation_artist;
        m_compilation_artist_sort = lcompilation_artist_sort;
        m_formattedartist.clear(); m_formattedtitle.clear();
        ensureSortFields();
    }

    QString Album() const { return m_album; }
    QString AlbumSort() const { return m_album_sort; }
    void setAlbum(const QString &lalbum,
                  const QString &lalbum_sort = nullptr)
    {
        m_album = lalbum;
        m_album_sort = lalbum_sort;
        m_formattedartist.clear(); m_formattedtitle.clear();
        ensureSortFields();
    }

    QString Title() const { return m_title; }
    QString TitleSort() const { return m_title_sort; }
    void setTitle(const QString &ltitle,
                  const QString &ltitle_sort = nullptr)
    {
        m_title = ltitle;
        m_title_sort = ltitle_sort;
        ensureSortFields();
    }

    QString FormatArtist();
    QString FormatTitle();

    QString Genre() const { return m_genre; }
    void setGenre(const QString &lgenre) { m_genre = lgenre; }

    void setDirectoryId(int ldirectoryid) { m_directoryid = ldirectoryid; }
    int getDirectoryId();

    void setArtistId(int lartistid) { m_artistid = lartistid; }
    int getArtistId();

    void setAlbumId(int lalbumid) { m_albumid = lalbumid; }
    int getAlbumId();

    void setGenreId(int lgenreid) { m_genreid = lgenreid; }
    int getGenreId();

    int Year() const { return m_year; }
    void setYear(int lyear) { m_year = lyear; }

    int Track() const { return m_tracknum; }
    void setTrack(int ltrack) { m_tracknum = ltrack; }

    int GetTrackCount() const { return m_trackCount; }
    void setTrackCount(int ltrackcount) { m_trackCount = ltrackcount; }

    int Length() const { return m_length; }
    void setLength(int llength) { m_length = llength; }

    int DiscNumber() const {return m_discnum;}
    void setDiscNumber(int discnum) { m_discnum = discnum; }

    int DiscCount() const {return m_disccount;}
    void setDiscCount(int disccount) { m_disccount = disccount; }

    int Playcount() const { return m_playcount; }
    void setPlaycount(int lplaycount) { m_playcount = lplaycount; }

    IdType ID() const { return m_id; }
    void setID(IdType lid) { m_id = lid; }
    void setRepo(RepoType repo) { m_id = (m_id & METADATA_ID_MASK) | (repo << METADATA_REPO_SHIFT); }

    bool isCDTrack(void) const { return ID_TO_REPO(m_id) == RT_CD; }
    bool isDBTrack(void) const { return ID_TO_REPO(m_id) == RT_Database; }
    bool isRadio(void) const { return ID_TO_REPO(m_id) == RT_Radio; }

    QString Filename(bool find = true);
    void setFilename(const QString &lfilename);
    QString getLocalFilename(void);

    QString Hostname(void) { return m_hostname; }
    void setHostname(const QString &host) { m_hostname = host; }

    uint64_t FileSize() const { return m_fileSize; }
    void setFileSize(uint64_t lfilesize) { m_fileSize = lfilesize; }

    QString Format() const { return m_format; }
    void setFormat(const QString &lformat) { m_format = lformat; }

    int Rating() const { return m_rating; }
    void decRating();
    void incRating();
    void setRating(int lrating) { m_rating = lrating; }

    QDateTime LastPlay() const { return m_lastplay; }
    void setLastPlay();
    void setLastPlay(QDateTime lastPlay);

    int PlayCount() const { return m_playcount; }
    void incPlayCount();

    // track is part of a compilation album
    bool Compilation() const { return m_compilation; }
    void setCompilation(bool state)
    {
        m_compilation = state;
        m_formattedartist.clear();
        m_formattedtitle.clear();
    }
    bool determineIfCompilation(bool cd = false);

    // for radio streams
    void setBroadcaster(const QString &broadcaster) { m_broadcaster = broadcaster; }
    QString Broadcaster(void) { return m_broadcaster; }

    void setChannel(const QString &channel) { m_channel = channel; }
    QString Channel(void) { return m_channel; }

    void setDescription(const QString &description) { m_description = description; }
    QString Description(void) { return m_description; }

    void setUrl(const QString &url, uint index = 0);
    QString Url(uint index = 0);

    void setLogoUrl(const QString &logourl) { m_logoUrl = logourl; }
    QString LogoUrl(void) { return m_logoUrl; }

    void setMetadataFormat(const QString &metaformat) { m_metaFormat = metaformat; }
    QString MetadataFormat(void) { return m_metaFormat; }

    void setCountry(const QString &country) { m_country = country; }
    QString Country(void) { return m_country; }

    void setLanguage(const QString &language) { m_language = language; }
    QString Language(void) { return m_language; }

    void setEmbeddedAlbumArt(AlbumArtList &albumart);

    void reloadMetadata(void);
    void dumpToDatabase(void);
    void setField(const QString &field, const QString &data);
    void getField(const QString& field, QString *data);
    void toMap(InfoMap &metadataMap, const QString &prefix = "");

    void persist(void);

    bool hasChanged(void) const { return m_changed; }

    bool compare(MusicMetadata *mdata) const;

    // static functions
    static MusicMetadata *createFromFilename(const QString &filename);
    static MusicMetadata *createFromID(int trackid);
    static void setArtistAndTrackFormats();
    static QStringList fillFieldList(QString field);
    static bool updateStreamList(void);

    // this looks for any image available - preferring a front cover if available
    QString getAlbumArtFile(void);
    // this looks only for the given image type
    QString getAlbumArtFile(ImageType type);

    AlbumArtImages *getAlbumArtImages(void);
    void reloadAlbumArtImages(void);

    LyricsData *getLyricsData(void);

    MetaIO *getTagger(void);

  private:
    void setCompilationFormatting(bool cd = false);
    QString formatReplaceSymbols(const QString &format);
    void checkEmptyFields(void);
    void ensureSortFields(void);
    void saveHostname(void);

    QString m_artist;
    QString m_artist_sort;
    QString m_compilation_artist;
    QString m_compilation_artist_sort;
    QString m_album;
    QString m_album_sort;
    QString m_title;
    QString m_title_sort;
    QString m_formattedartist;
    QString m_formattedtitle;
    QString m_genre;
    QString m_genre_sort;
    QString m_format;
    int     m_year             {0};
    int     m_tracknum         {0};
    int     m_trackCount       {0};
    int     m_discnum          {0};
    int     m_disccount        {0};
    int     m_length           {0};
    int     m_rating           {0};
    int     m_directoryid      {-1};
    int     m_artistid         {-1};
    int     m_compartistid     {-1};
    int     m_albumid          {-1};
    int     m_genreid          {-1};
    QDateTime m_lastplay;
    QDateTime m_templastplay;
    QDateTime m_dateadded;
    int  m_playcount           {0};
    int  m_tempplaycount       {0};
    bool m_compilation         {false};

    AlbumArtImages *m_albumArt {nullptr};

    LyricsData *m_lyricsData   {nullptr};

    IdType   m_id;
    QString  m_filename;       // file name as stored in the DB
    QString  m_hostname;       // host where file is located as stored in the DB
    QString  m_actualFilename; // actual URL of the file if found
    uint64_t m_fileSize        {0};
    bool     m_changed         {false};

    // radio stream stuff
    QString m_broadcaster;
    QString m_channel;
    QString m_description;
    UrlList m_urls;
    QString m_logoUrl;
    QString m_metaFormat;
    QString m_country;
    QString m_language;

    // Various formatting strings
    static QString m_formatnormalfileartist;
    static QString m_formatnormalfiletrack;
    static QString m_formatnormalcdartist;
    static QString m_formatnormalcdtrack;

    static QString m_formatcompilationfileartist;
    static QString m_formatcompilationfiletrack;
    static QString m_formatcompilationcdartist;
    static QString m_formatcompilationcdtrack;
};

bool operator==(MusicMetadata& a, MusicMetadata& b);
bool operator!=(MusicMetadata& a, MusicMetadata& b);

Q_DECLARE_METATYPE(MusicMetadata *)
Q_DECLARE_METATYPE(MusicMetadata)

typedef QList<MusicMetadata*> MetadataPtrList;
Q_DECLARE_METATYPE(MetadataPtrList *)
Q_DECLARE_METATYPE(ImageType);

//---------------------------------------------------------------------------

class META_PUBLIC MetadataLoadingThread : public MThread
{

  public:

    explicit MetadataLoadingThread(AllMusic *parent_ptr)
        : MThread("MetadataLoading"), parent(parent_ptr) {}
    void run() override; // MThread

  private:

    AllMusic *parent {nullptr};
};

//---------------------------------------------------------------------------

class META_PUBLIC AllMusic
{
    Q_DECLARE_TR_FUNCTIONS(AllMusic);

  public:

    AllMusic(void);
    ~AllMusic();

    MusicMetadata*   getMetadata(int an_id);
    bool        updateMetadata(int an_id, MusicMetadata *the_track);
    int         count() const { return m_numPcs; }
    int         countLoaded() const { return m_numLoaded; }
    void        save();
    bool        startLoading(void);
    void        resync();   //  After a CD rip, for example

    // cd stuff
    void        clearCDData(void);
    void        addCDTrack(const MusicMetadata &the_track);
    bool        checkCDTrack(MusicMetadata *the_track);
    MusicMetadata*   getCDMetadata(int m_the_track);
    QString     getCDTitle(void) const { return m_cdTitle; }
    void        setCDTitle(const QString &a_title) { m_cdTitle = a_title; }
    int         getCDTrackCount(void) const { return m_cdData.count(); }

    bool        doneLoading() const { return m_done_loading; }
    bool        cleanOutThreads();

    MetadataPtrList *getAllMetadata(void) { return &m_all_music; }
    MetadataPtrList *getAllCDMetadata(void) { return &m_cdData; }

    bool isValidID(int an_id);

  private:
    MetadataPtrList     m_all_music;

    int m_numPcs                               {0};
    int m_numLoaded                            {0};

    typedef QMap<int, MusicMetadata*> MusicMap;
    MusicMap m_music_map;

    // cd stuff
    MetadataPtrList m_cdData; //  More than one cd player?
    QString m_cdTitle;

    MetadataLoadingThread   *m_metadata_loader {nullptr};
    bool                     m_done_loading    {false};

    int                      m_playcountMin    {0};
    int                      m_playcountMax    {0};
#if QT_VERSION < QT_VERSION_CHECK(5,8,0)
    double                   m_lastplayMin     {0.0};
    double                   m_lastplayMax     {0.0};
#else
    qint64                   m_lastplayMin     {0};
    qint64                   m_lastplayMax     {0};
#endif
};

typedef QList<MusicMetadata*> StreamList;

class META_PUBLIC AllStream
{
  public:

    AllStream(void);
    ~AllStream();

    void loadStreams(void);

    bool isValidID(MusicMetadata::IdType an_id);

    MusicMetadata*   getMetadata(MusicMetadata::IdType an_id);

    StreamList *getStreams(void) { return &m_streamList; }

    void addStream(MusicMetadata *mdata);
    void removeStream(MusicMetadata *mdata);
    void updateStream(MusicMetadata *mdata);

  private:
    StreamList m_streamList;
};

//----------------------------------------------------------------------------

class AlbumArtScannerThread: public MThread
{
  public:
    explicit AlbumArtScannerThread(QStringList strList) :
            MThread("AlbumArtScanner"), m_strList(strList) {}

    void run() override // MThread
    {
        RunProlog();
        gCoreContext->SendReceiveStringList(m_strList);
        RunEpilog();
    }

    QStringList getResult(void) { return m_strList; }

  private:
    QStringList m_strList;
};

class META_PUBLIC AlbumArtImages
{
    Q_DECLARE_TR_FUNCTIONS(AlbumArtImages);

  public:
    AlbumArtImages(MusicMetadata *metadata, bool loadFromDB = true);
    ~AlbumArtImages();

    void           scanForImages(void);
    void           addImage(const AlbumArtImage &newImage);
    uint           getImageCount() { return m_imageList.size(); }
    AlbumArtImage *getImage(ImageType type);
    AlbumArtImage *getImageByID(int imageID);
    QStringList    getImageFilenames(void) const;
    AlbumArtList  *getImageList(void) { return &m_imageList; }
    AlbumArtImage *getImageAt(uint index);

    void dumpToDatabase(void);

    static ImageType guessImageType(const QString &filename);
    static QString   getTypeName(ImageType type);
    static QString   getTypeFilename(ImageType type);
    static ImageType getImageTypeFromName(const QString &name);

  private:
    void findImages(void);

    MusicMetadata *m_parent {nullptr};
    AlbumArtList   m_imageList;
};

Q_DECLARE_METATYPE(AlbumArtImage*);

#endif
