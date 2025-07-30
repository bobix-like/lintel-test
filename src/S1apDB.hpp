#ifndef S1AP_DB
#define S1AP_DB

#include <expected>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

template <typename T>
concept IsCGI = std::is_same_v<std::decay_t<T>, std::vector<unsigned char>>;

template <typename T>
concept IsOptCgi = std::is_same_v<std::decay_t<T>, std::optional<std::vector<unsigned char>>>;

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

    template <IsCGI CGI>
    static Event CreateAttachRequestWithImsi(const unsigned long timestamp,
                                             const unsigned long imsi,
                                             const unsigned int enodebID,
                                             CGI&& cgi);

    template <IsCGI CGI>
    static Event CreateAttachRequestWithMTmsi(const unsigned long timestamp,
                                              const unsigned int enodebID,
                                              const unsigned int mTmsi,
                                              CGI&& cgi);

    template <IsCGI CGI>
    static Event CreateIdentityResponse(const unsigned long timestamp,
                                        const unsigned long imsi,
                                        const unsigned int enodebID,
                                        const unsigned int mmeID,
                                        CGI&& cgi);

    static Event CreateAttachAccept(const unsigned long timestamp,
                                    const unsigned int enodebID,
                                    const unsigned int mmeID,
                                    const unsigned int mTmsi);

    template <IsCGI CGI>
    static Event CreatePaging(const unsigned long timestamp,
                              const unsigned int mTmsi,
                              CGI&& cgi);

    template <IsCGI CGI>
    static Event CreatePathSwitchRequest(const unsigned long timestamp,
                                         const unsigned int enodebID,
                                         const unsigned int mmeID,
                                         CGI&& cgi);

    static Event CreatePathSwitchRequestAcknowledge(const unsigned long timestamp,
                                                    const unsigned int enodebID,
                                                    const unsigned int mmeID);

    template <IsCGI CGI>
    static Event CreateUEContextReleaseCommand(const unsigned long timestamp,
                                               const unsigned int enodebID,
                                               const unsigned int mmeID,
                                               CGI&& cgi);

    static Event CreateUEContextReleaseResponse(const unsigned long timestamp,
                                                const unsigned int enodebID,
                                                const unsigned int mmeID);

    const Type& GetType() const;
    const unsigned long& GetTimestamp() const;
    const std::optional<std::vector<unsigned char>>& GetCgi() const;
    const std::optional<unsigned long>& GetImsi() const;
    const std::optional<unsigned int>& GetEnodebID() const;
    const std::optional<unsigned int>& GetMmeID() const;
    const std::optional<unsigned int>& GetMTmci() const;

    enum class Error 
    {
      WrongEventType,
      WrongImsiAndMTmsiArgs,
      ImsiNotExist,
      BadImsi,
      BadEnodebID,
      BadMTmsi,
      BadMmeID,
      BadCgi,
    };

    std::expected<void, Error> Verify() const;

  private:
    Type type_;
    unsigned long timestamp_;

    std::optional<std::vector<unsigned char>> cgi_      = std::nullopt;
    std::optional<unsigned long>              imsi_     = std::nullopt;
    std::optional<unsigned int>               enodebID_ = std::nullopt;
    std::optional<unsigned int>               mmeID_    = std::nullopt;
    std::optional<unsigned int>               mTmci_    = std::nullopt;

    std::expected<void, Error> VerifyAttachRequest() const;
    std::expected<void, Error> VerifyIdentityResponse() const;
    std::expected<void, Error> validateAttachAccept() const;
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

    template <IsOptCgi CGI>
    S1apOut(const Type type,
            const unsigned long imsi,
            CGI&& cgi)
    : type_(type),
      imsi_(imsi),
      cgi_(std::forward<CGI>(cgi)) {}

    Type GetType() const;
    unsigned long GetImsi() const;
    const std::optional<std::vector<unsigned char>>& GetCgi_() const;

  private:
    Type type_;
    unsigned long imsi_;
    std::optional<std::vector<unsigned char>> cgi_;
};

class S1apDB final
{
  public:
    enum class Error
    {
      ImsiNotExists,
    };

    using HandleError = std::variant<Error, Event::Error>;
    using HandleOut = std::expected<std::optional<S1apOut>, HandleError>;
    HandleOut Handle(const Event& event);

    static S1apDB& GetInstance();

  private:
    S1apDB() = default;

    // NOTE: TODO: add perfect forwarding for moving cgi from event
    HandleOut HandleAttachRequestWithImsi(const Event& event);
    HandleOut HandleAttachRequestWithMTmsi(const Event& event);
    HandleOut HandleIdentityResponse(const Event& event);
    HandleOut HandleAttachAccept(const Event& event);
    HandleOut HandlePaging(const Event& event);
    HandleOut HandlePathSwitchRequest(const Event& event);
    HandleOut HandlePathSwitchRequestAcknowledge(const Event& event);
    HandleOut HandleUEContextReleaseCommand(const Event& event);
    HandleOut HandleUEContextReleaseResponse(const Event& event);

    std::expected<unsigned long, HandleError> ResolveImsi(const Event& event);

    class Subscriber
    {
      public:
        enum class State
        {
          DETACHED,
          ATTACHED,
        };

        void SetLastEvent(const Event::Type eventType, const unsigned long timestamp);
        void SetMTmsi(const unsigned int mTmsi);
        void SetEnodebID(const unsigned int enodebID);
        void SetMmeID(const unsigned int mmeID);
        void SetState(const State state);

        template <IsCGI CGI>
        void SetCgi(CGI&& cgi);

        std::optional<unsigned long> GetImsi() const;
        std::optional<unsigned int> GetMTmsi() const;
        std::optional<unsigned int> GetEnodebID() const;
        std::optional<unsigned int> GetMmeID() const;
        const std::optional<std::vector<unsigned char>>& GetCgi() const;

        Event::Type GetLastEventType() const;
        unsigned long GetLastEventTimestamp() const;

        State GetState() const;

      private:
        std::optional<unsigned long> imsi_    = std::nullopt;
        std::optional<unsigned int> mTmsi_    = std::nullopt;
        std::optional<unsigned int> enodebID_ = std::nullopt;
        std::optional<unsigned int> mmeID_    = std::nullopt;

        std::optional<std::vector<unsigned char>> cgi_ = std::nullopt;

        State state_ = State::DETACHED;

        Event::Type eventType_;
        unsigned long lastEventTimestamp_;
    };

    std::unordered_map<unsigned long, Subscriber> imsiToSubscriber;
    std::unordered_map<unsigned int, unsigned long> mTmsiToImsi;
    std::unordered_map<unsigned int, unsigned long> enodebIDToImsi;
    std::unordered_map<unsigned int, unsigned long> mmeIDToImsi;

};

#endif // S1AP_DB

