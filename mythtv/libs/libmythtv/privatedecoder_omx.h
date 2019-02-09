#ifndef PRIVATEDECODER_OMX_H
#define PRIVATEDECODER_OMX_H

#include "privatedecoder.h"

#include <OMX_Types.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#ifdef USING_BROADCOM
#include <OMX_Broadcom.h>
#endif

#include <QMutex>
#include <QSemaphore>
#include <QVector>
#include <QList>

#include "omxcontext.h"

class PrivateDecoderOMX : public PrivateDecoder, private OMXComponentCtx
{
  public:
    static void GetDecoders(render_opts &opts);
    static QString const s_name; // ="openmax"

    PrivateDecoderOMX();
    virtual ~PrivateDecoderOMX() override;

    // PrivateDecoder implementation
    QString GetName(void) override; // PrivateDecoder
    bool Init(const QString &decoder,
              PlayerFlags flags,
              AVCodecContext *avctx) override; // PrivateDecoder
    bool Reset(void) override; // PrivateDecoder
    int  GetFrame(AVStream *stream,
                  AVFrame *picture,
                  int *got_picture_ptr,
                  AVPacket *pkt) override; // PrivateDecoder
    bool HasBufferedFrames(void) override; // PrivateDecoder
    bool NeedsReorderedPTS(void)  override // PrivateDecoder
        { return true; }

  private:
    // OMXComponentCtx implementation
    OMX_ERRORTYPE Event(OMXComponent&, OMX_EVENTTYPE, OMX_U32, OMX_U32, OMX_PTR) override; // OMXComponentCtx
    OMX_ERRORTYPE EmptyBufferDone(OMXComponent&, OMX_BUFFERHEADERTYPE*) override; // OMXComponentCtx
    OMX_ERRORTYPE FillBufferDone(OMXComponent&, OMX_BUFFERHEADERTYPE*) override; // OMXComponentCtx
    void ReleaseBuffers(OMXComponent&) override; // OMXComponentCtx

  private:
    // OMXComponentCB actions
    typedef OMX_ERRORTYPE ComponentCB();
    ComponentCB AllocBuffersCB;
    ComponentCB FreeOutputBuffersCB, AllocOutputBuffersCB;
    ComponentCB UseBuffersCB;

    OMX_ERRORTYPE FillOutputBuffers();
    OMX_ERRORTYPE SettingsChanged(AVCodecContext *);
    OMX_ERRORTYPE SetNalType(AVCodecContext *);
    OMX_ERRORTYPE GetAspect(OMX_CONFIG_POINTTYPE &, int index) const;
#ifdef USING_BROADCOM
    OMX_ERRORTYPE GetInterlace(OMX_CONFIG_INTERLACETYPE &, int index) const;
#endif
    int ProcessPacket(AVStream *, AVPacket *);
    int GetBufferedFrame(AVStream *, AVFrame *);
    bool CreateFilter(AVCodecContext *);

  private:
    OMXComponent m_videc;
    AVBSFContext *m_bitstreamFilter;
    bool m_bStartTime;
    AVCodecContext *m_avctx;

    QMutex mutable m_lock;      // Protects data following
    bool m_bSettingsChanged;
    bool m_bSettingsHaveChanged;

    // video decoder buffers
    QSemaphore m_ibufs_sema;    // EmptyBufferCB signal
    QSemaphore m_obufs_sema;    // FillBufferCB signal
    QList<OMX_BUFFERHEADERTYPE*> m_ibufs, m_obufs;
};

#endif // ndef PRIVATEDECODER_OMX_H
/* EOF */
