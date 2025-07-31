#ifndef S1AP_DB_HPP
#define S1AP_DB_HPP

#include <expected>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace S1ap
{
  using Timestamp = unsigned long;

  using Imsi  = unsigned long;
  using OImsi = std::optional<Imsi>;

  using MTmsi  = unsigned int;
  using OMTmsi = std::optional<MTmsi>;

  using EnodebID  = unsigned int;
  using OEnodebID = std::optional<EnodebID>;

  using MmeID  = unsigned int;
  using OMmeID = std::optional<MmeID>;

  using Cgi  = std::vector<unsigned char>;
  using OCgi = std::optional<Cgi>;
}


template <typename T>
concept IsCgi = std::is_same_v<std::decay_t<T>, S1ap::Cgi>;

template <typename T>
concept IsOptionalCgi = std::is_same_v<std::decay_t<T>, S1ap::OCgi>;

template <typename T>
concept IsCgiCompatible = IsCgi<T> || IsOptionalCgi<T>;

class Event final
{
  public:
    enum class Type
    {
      AttachRequest,                 // ENODEB --> MME
      IdentityResponse,              // ENODEB --> MME
      AttachAccept,                  // MME    --> ENODEB
      Paging,                        // ENODEB --> MME
      PathSwitchRequest,             // ENODEB --> MME
      PathSwitchRequestAcknowledge,  // MME    --> ENODEB
      UEContextReleaseCommand,       // ENODEB --> MME
      UEContextReleaseResponse,      // MME    --> ENODEB
    };

    template <IsCgiCompatible CgiArg>
    static Event CreateAttachRequestWithImsi(const S1ap::Timestamp timestamp,
                                             const S1ap::Imsi imsi,
                                             const S1ap::EnodebID enodebID, 
                                             CgiArg&& cgi);
    template <IsCgiCompatible CgiArg>
    static Event CreateAttachRequestWithMTmsi(const S1ap::Timestamp timestamp,
                                              const S1ap::EnodebID enodebID,
                                              const S1ap::MTmsi mTmsi,
                                              CgiArg&& cgi);

    template <IsCgiCompatible CgiArg>
    static Event CreateIdentityResponse(const S1ap::Timestamp timestamp,
                                        const S1ap::Imsi imsi,
                                        const S1ap::EnodebID enodebID,
                                        const S1ap::MmeID mmeID,
                                        CgiArg&& cgi);

    static Event CreateAttachAccept(const S1ap::Timestamp timestamp,
                                    const S1ap::EnodebID enodebID,
                                    const S1ap::MmeID mmeID,
                                    const S1ap::MTmsi mTmsi);

    template <IsCgiCompatible CgiArg>
    static Event CreatePaging(const S1ap::Timestamp timestamp,
                              const S1ap::MTmsi mTmsi,
                              CgiArg&& cgi);

    template <IsCgiCompatible CgiArg>
    static Event CreatePathSwitchRequest(const S1ap::Timestamp timestamp,
                                         const S1ap::EnodebID enodebID,
                                         const S1ap::MmeID mmeID,
                                         CgiArg&& cgi);

    static Event CreatePathSwitchRequestAcknowledge(const S1ap::Timestamp timestamp,
                                                    const S1ap::EnodebID enodebID,
                                                    const S1ap::MmeID mmeID);

    template <IsCgiCompatible CgiArg>
    static Event CreateUEContextReleaseCommand(const S1ap::Timestamp timestamp,
                                               const S1ap::EnodebID enodebID,
                                               const S1ap::MmeID mmeID,
                                               CgiArg&& cgi);

    static Event CreateUEContextReleaseResponse(const S1ap::Timestamp timestamp,
                                                const S1ap::EnodebID enodebID,
                                                const S1ap::MmeID mmeID);

    const Type& GetType() const;
    const S1ap::Timestamp& GetTimestamp() const;
    const S1ap::OCgi& GetCgi() const;
    const S1ap::OImsi& GetImsi() const;
    const S1ap::OEnodebID& GetEnodebID() const;
    const S1ap::OMmeID& GetMmeID() const;
    const S1ap::OMTmsi& GetMTmsi() const;

    enum class Error
    {
      WrongEventType,
      WrongImsiAndMTmsiArgs,
      ImsiNotExist,
      MTmsiNotExist,
      BadImsi,
      BadEnodebID,
      BadMTmsi,
      BadMmeID,
      BadCgi,
      MissingImsiOrMTmsi,
    };

    using VerifyOut = std::expected<void, Error>;
    VerifyOut Verify() const;

  private:
    Event() = default;

    Type type_;
    S1ap::Timestamp timestamp_;

    S1ap::OCgi cgi_            = std::nullopt;
    S1ap::OImsi imsi_          = std::nullopt;
    S1ap::OEnodebID enodebID_  = std::nullopt;
    S1ap::OMmeID mmeID_        = std::nullopt;
    S1ap::OMTmsi mTmsi_        = std::nullopt;

    VerifyOut VerifyAttachRequest() const;
    VerifyOut VerifyIdentityRequest() const;
    VerifyOut VerifyIdentityResponse() const;
    VerifyOut VerifyAttachAccept() const;
    VerifyOut VerifyPaging() const;
    VerifyOut VerifyPathSwitchRequest() const;
    VerifyOut VerifyPathSwitchRequestAcknowledge() const;
    VerifyOut VerifyUEContextReleaseCommand() const;
    VerifyOut VerifyUEContextReleaseResponse() const;
};

class S1apOut final
{
  public:
    enum class Type
    {
      Reg,
      UnReg,
      CgiChange
    };

    template <IsCgiCompatible CgiArg>
    S1apOut(const Type type,
            const S1ap::Imsi imsi,
            CgiArg&& cgi)
    : type_(type),
      imsi_(imsi),
      cgi_(std::forward<CgiArg>(cgi)) {}

    Type GetType() const;
    S1ap::Imsi GetImsi() const;
    const S1ap::OCgi& GetCgi() const;

  private:
    Type type_;
    S1ap::Imsi imsi_;
    S1ap::OCgi cgi_ = std::nullopt;
};

class S1apDB final
{
  public:
    enum class Error
    {
      ImsiNotExists,
      MTmsiNotExists,
      SubscriberNotFound,
      InvalidStateForEvent,
      NoImsiOrMTmsiInEvent,
      TimeoutOccurred,
      WrongState,
    };

    using HandleError = std::variant<Error, Event::Error>;
    using HandleOut   = std::expected<std::optional<S1apOut>, HandleError>;

    HandleOut Handle(const Event& event);
    void HandleTimeouts(S1ap::Timestamp currentTimestamp);

    static S1apDB& GetInstance();

  private:
    S1apDB() = default;

    HandleOut HandleAttachRequest(const Event& event);
    HandleOut HandleIdentityResponse(const Event& event);
    HandleOut HandleAttachAccept(const Event& event);
    HandleOut HandlePaging(const Event& event);
    HandleOut HandlePathSwitchRequest(const Event& event);
    HandleOut HandlePathSwitchRequestAcknowledge(const Event& event);
    HandleOut HandleUEContextReleaseCommand(const Event& event);
    HandleOut HandleUEContextReleaseResponse(const Event& event);

    S1ap::MTmsi GenerateNewMTmsi();

    S1ap::MTmsi nextMTmsi_ = 1000; 

    class Subscriber
    {
      public:
        enum class State
        {
          DETACHED,
          ATTACHING,
          ATTACHED,
          PAGING_STATE,
          SERVICE_REQUEST_PENDING,
          HANDOVER_STATE,
          RELEASING,
        };

        void SetLastEvent(const Event::Type eventType, const S1ap::Timestamp timestamp);
        void SetMTmsi(const S1ap::MTmsi mTmsi);
        void SetEnodebID(const S1ap::EnodebID enodebID);
        void SetMmeID(const S1ap::MmeID mmeID);
        void SetState(const State state);

        template <IsCgiCompatible CgiArg>
        void SetCgi(CgiArg&& cgi);

        void SetImsi(S1ap::Imsi imsi);

        S1ap::OImsi GetImsi() const;
        S1ap::OMTmsi GetMTmsi() const;
        S1ap::OEnodebID GetEnodebID() const;
        S1ap::OMmeID GetMmeID() const;
        const S1ap::OCgi& GetCgi() const;

        Event::Type GetLastEventType() const;
        S1ap::Timestamp GetLastEventTimestamp() const;

        State GetState() const;

      private:
        S1ap::OImsi imsi_          = std::nullopt;
        S1ap::OMTmsi mTmsi_        = std::nullopt;
        S1ap::OEnodebID enodebID_  = std::nullopt;
        S1ap::OMmeID mmeID_        = std::nullopt;

        S1ap::OCgi cgi_ = std::nullopt;

        State state_ = State::DETACHED;

        Event::Type eventType_;
        S1ap::Timestamp lastEventTimestamp_;
    };

    std::expected<S1ap::Imsi, HandleError> ResolveImsiFromEvent(const Event& event) const;
    std::expected<S1ap::Imsi, HandleError> ResolveImsiFromEnodebID(S1ap::EnodebID enodebID) const;
    void DetachSubscriber(Subscriber& subscriber);

    HandleOut ProcessNewAttach(const Event& event);
    HandleOut ProcessExistingAttach(Subscriber& subscriber, const Event& event);
    HandleOut ProcessDuplicateAttach(Subscriber& subscriber, const Event& event);
    HandleOut ProcessIdentityResponseForNewUser(const Event& event);
    HandleOut ProcessIdentityResponseForAttachingUser(Subscriber& subscriber, const Event& event);
    HandleOut ProcessPagingRequest(Subscriber& subscriber, const Event& event);
    HandleOut ProcessPathSwitchRequest(Subscriber& subscriber, const Event& event);
    HandleOut ProcessUEContextRelease(Subscriber& subscriber, const Event& event);

    std::unordered_map<S1ap::Imsi, Subscriber> imsiToSubscriber;
    std::unordered_map<S1ap::MTmsi, S1ap::Imsi> mTmsiToImsi;
    std::unordered_map<S1ap::EnodebID, S1ap::Imsi> enodebIDToImsi;
    std::unordered_map<S1ap::MmeID, S1ap::Imsi> mmeIDToImsi;

    std::unordered_map<S1ap::Imsi, S1ap::Timestamp> imsiToIdentityRequestTimeout_;
    const S1ap::Timestamp IDENTITY_RESPONSE_TIMEOUT_MS = 5000;
};

#endif // S1AP_DB_HPP
