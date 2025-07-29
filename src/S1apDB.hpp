#ifndef S1AP_DB
#define S1AP_DB

#include <expected>
#include <map>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <vector>

template <typename T>
concept IsCGI = std::is_same_v<std::decay<T>, std::vector<unsigned char>>;

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
                                               CGI cgi);

    static Event CreateUEContextReleaseResponse(const unsigned long timestamp,
                                                const unsigned int enodebID,
                                                const unsigned int mmeID);

  private:
    Type type_;
    unsigned long timestamp_;

    std::optional<std::vector<unsigned char>> cgi_;

    std::optional<unsigned long> imsi_;

    std::optional<unsigned int> enodebID_;
    std::optional<unsigned int> mmeID_;
    std::optional<unsigned int> mTmci_;

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

    template <IsCGI CGI>
    S1apOut(const Type type,
            const unsigned long imsi,
            CGI&& cgi)
    : type_(type),
      imsi_(imsi),
      cgi_(std::forward(cgi)) {}

    Type GetType();
    unsigned long GetImsi();
    const std::vector<unsigned char>& GetCgi_;

  private:
    Type type_;
    unsigned long imsi_;
    std::vector<unsigned char> cgi_;
};

class S1apDB final
{
  public:
    enum class Error {/* TODO: */};
    std::expected<std::optional<S1apOut>, Error> Handle(const Event& event);

  private:
    class Subscriber {/* TODO: */};
    // TODO:
};

#endif // S1AP_DB

