#ifndef S1AP_DB_HPP
#define S1AP_DB_HPP

#include <expected>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

template <typename T>
concept IsCgi = std::is_same_v<std::decay_t<T>, std::vector<unsigned char>>;

template <typename T>
concept IsOptionalCgi = std::is_same_v<std::decay_t<T>, std::optional<std::vector<unsigned char>>>;

template <typename T>
concept IsCgiCompatible = IsCgi<T> || IsOptionalCgi<T>;

class Event final
{
  public:
    enum class Type
    {
      AttachRequest,                // ENODEB --> MME
      IdentityResponse,             // ENODEB --> MME
      AttachAccept,                 // MME    --> ENODEB
      Paging,                       // ENODEB --> MME
      PathSwitchRequest,            // ENODEB --> MME
      PathSwitchRequestAcknowledge, // MME    --> ENODEB
      UEContextReleaseCommand,      // ENODEB --> MME
      UEContextReleaseResponse,     // MME    --> ENODEB
    };

    template <IsCgiCompatible CgiArg>
    static Event CreateAttachRequestWithImsi(const unsigned long timestamp,
                                             const unsigned long imsi,
                                             const unsigned int enodebID, CgiArg&& cgi);
    template <IsCgiCompatible CgiArg>
    static Event CreateAttachRequestWithMTmsi(const unsigned long timestamp,
                                              const unsigned int enodebID,
                                              const unsigned int mTmsi,
                                              CgiArg&& cgi);

    template <IsCgiCompatible CgiArg>
    static Event CreateIdentityResponse(const unsigned long timestamp,
                                        const unsigned long imsi,
                                        const unsigned int enodebID,
                                        const unsigned int mmeID,
                                        CgiArg&& cgi);

    static Event CreateAttachAccept(const unsigned long timestamp,
                                    const unsigned int enodebID,
                                    const unsigned int mmeID,
                                    const unsigned int mTmsi);

    template <IsCgiCompatible CgiArg>
    static Event CreatePaging(const unsigned long timestamp,
                              const unsigned int mTmsi,
                              CgiArg&& cgi);

    template <IsCgiCompatible CgiArg>
    static Event CreatePathSwitchRequest(const unsigned long timestamp,
                                         const unsigned int enodebID,
                                         const unsigned int mmeID,
                                         CgiArg&& cgi);

    static Event CreatePathSwitchRequestAcknowledge(const unsigned long timestamp,
                                                    const unsigned int enodebID,
                                                    const unsigned int mmeID);

    template <IsCgiCompatible CgiArg>
    static Event CreateUEContextReleaseCommand(const unsigned long timestamp,
                                               const unsigned int enodebID,
                                               const unsigned int mmeID,
                                               CgiArg&& cgi);

    static Event CreateUEContextReleaseResponse(const unsigned long timestamp,
                                                const unsigned int enodebID,
                                                const unsigned int mmeID);

    const Type& GetType() const;
    const unsigned long& GetTimestamp() const;
    const std::optional<std::vector<unsigned char>>& GetCgi() const;
    const std::optional<unsigned long>& GetImsi() const;
    const std::optional<unsigned int>& GetEnodebID() const;
    const std::optional<unsigned int>& GetMmeID() const;
    const std::optional<unsigned int>& GetMTmsi() const;

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

    std::expected<void, Error> Verify() const;

  private:
    Event() = default;

    Type type_;
    unsigned long timestamp_;

    std::optional<std::vector<unsigned char>> cgi_      = std::nullopt;
    std::optional<unsigned long>              imsi_     = std::nullopt;
    std::optional<unsigned int>               enodebID_ = std::nullopt;
    std::optional<unsigned int>               mmeID_    = std::nullopt;
    std::optional<unsigned int>               mTmsi_    = std::nullopt;

    std::expected<void, Error> VerifyAttachRequest() const;
    std::expected<void, Error> VerifyIdentityRequest() const;
    std::expected<void, Error> VerifyIdentityResponse() const;
    std::expected<void, Error> VerifyAttachAccept() const;
    std::expected<void, Error> VerifyPaging() const;
    std::expected<void, Error> VerifyPathSwitchRequest() const;
    std::expected<void, Error> VerifyPathSwitchRequestAcknowledge() const;
    std::expected<void, Error> VerifyUEContextReleaseCommand() const;
    std::expected<void, Error> VerifyUEContextReleaseResponse() const;
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

    template <IsOptionalCgi CgiArg>
    S1apOut(const Type type,
            const unsigned long imsi,
            CgiArg&& cgi)
    : type_(type),
      imsi_(imsi),
      cgi_(std::forward<CgiArg>(cgi)) {}

    Type GetType() const;
    unsigned long GetImsi() const;
    const std::optional<std::vector<unsigned char>>& GetCgi() const;

  private:
    Type type_;
    unsigned long imsi_;
    std::optional<std::vector<unsigned char>> cgi_ = std::nullopt;
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
    };

    using HandleError = std::variant<Error, Event::Error>;
    using HandleOut   = std::expected<std::optional<S1apOut>, HandleError>;

    HandleOut Handle(const Event& event);
    void HandleTimeouts(unsigned long currentTimestamp);

    static S1apDB& GetInstance();

  private:
    S1apDB() = default;

    // NOTE: TODO: add perfect forwarding for moving cgi from event - это теперь реализовано
    // NOTE: it's a good idea to create class State which know how to handle itself. It forces single responcibility principle
    // also it wuld be good if I use such pattern when I verified Event
    HandleOut HandleAttachRequest(const Event& event);
    HandleOut HandleIdentityResponse(const Event& event);
    HandleOut HandleAttachAccept(const Event& event);
    HandleOut HandlePaging(const Event& event);
    HandleOut HandlePathSwitchRequest(const Event& event);
    HandleOut HandlePathSwitchRequestAcknowledge(const Event& event);
    HandleOut HandleUEContextReleaseCommand(const Event& event);
    HandleOut HandleUEContextReleaseResponse(const Event& event);

  // TODO: maybe generate m_tmsi isn't needed, to be honest this method is generated by GPT
    unsigned int GenerateNewMTmsi();

    unsigned int nextMTmsi_ = 1000; 

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

        void SetLastEvent(const Event::Type eventType, const unsigned long timestamp);
        void SetMTmsi(const unsigned int mTmsi);
        void SetEnodebID(const unsigned int enodebID);
        void SetMmeID(const unsigned int mmeID);
        void SetState(const State state);

        template <IsCgiCompatible CgiArg>
        void SetCgi(CgiArg&& cgi);

        void SetImsi(unsigned long imsi);

        std::optional<unsigned long> GetImsi() const;
        std::optional<unsigned int> GetMTmsi() const;
        std::optional<unsigned int> GetEnodebID() const;
        std::optional<unsigned int> GetMmeID() const;
        const std::optional<std::vector<unsigned char>>& GetCgi() const;

        Event::Type GetLastEventType() const;
        unsigned long GetLastEventTimestamp() const;

        State GetState() const;

      private:
        std::optional<unsigned long> imsi_     = std::nullopt;
        std::optional<unsigned int>  mTmsi_    = std::nullopt;
        std::optional<unsigned int>  enodebID_ = std::nullopt;
        std::optional<unsigned int>  mmeID_    = std::nullopt;

        std::optional<std::vector<unsigned char>> cgi_ = std::nullopt;

        State state_ = State::DETACHED;

        Event::Type eventType_;
        unsigned long lastEventTimestamp_;
    };

    std::expected<unsigned long, HandleError> ResolveImsiFromEvent(const Event& event) const;
    std::expected<unsigned long, HandleError> ResolveImsiFromEnodebID(unsigned int enodebID) const;
    void RetachSubscriber(Subscriber& subscriber);

    HandleOut ProcessNewAttach(const Event& event);
    HandleOut ProcessExistingAttach(Subscriber& subscriber, const Event& event);
    HandleOut ProcessDuplicateAttach(Subscriber& subscriber, const Event& event);
    HandleOut ProcessIdentityResponseForNewUser(const Event& event);
    HandleOut ProcessIdentityResponseForAttachingUser(Subscriber& subscriber, const Event& event);
    HandleOut ProcessPagingRequest(Subscriber& subscriber, const Event& event);
    HandleOut ProcessPathSwitchRequest(Subscriber& subscriber, const Event& event);
    HandleOut ProcessUEContextRelease(Subscriber& subscriber, const Event& event);

    std::unordered_map<unsigned long, Subscriber>   imsiToSubscriber;
    std::unordered_map<unsigned int, unsigned long> mTmsiToImsi;
    std::unordered_map<unsigned int, unsigned long> enodebIDToImsi;
    std::unordered_map<unsigned int, unsigned long> mmeIDToImsi;

    std::unordered_map<unsigned long, unsigned long> imsiToIdentityRequestTimeout_;
    const unsigned long IDENTITY_RESPONSE_TIMEOUT_MS = 5000;
};

#endif // S1AP_DB_HPP

