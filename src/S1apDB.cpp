#include "S1apDB.hpp"

#include <print>

const Event::Type& Event::GetType() const { return type_; }
const unsigned long& Event::GetTimestamp() const { return timestamp_; }
const std::optional<std::vector<unsigned char>>& Event::GetCgi() const { return cgi_; }
const std::optional<unsigned long>& Event::GetImsi() const { return imsi_; }
const std::optional<unsigned int>& Event::GetEnodebID() const { return enodebID_; }
const std::optional<unsigned int>& Event::GetMmeID() const { return mmeID_; }
const std::optional<unsigned int>& Event::GetMTmsi() const { return mTmsi_; }

template <IsCgiCompatible CgiArg>
Event Event::CreateAttachRequestWithImsi(const unsigned long timestamp,
                                         const unsigned long imsi,
                                         const unsigned int enodebID,
                                         CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::AttachRequest;
  event.timestamp_ = timestamp;
  event.imsi_ = imsi;
  event.enodebID_ = enodebID;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

template <IsCgiCompatible CgiArg>
Event Event::CreateAttachRequestWithMTmsi(const unsigned long timestamp,
                                          const unsigned int enodebID,
                                          const unsigned int mTmsi,
                                          CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::AttachRequest;
  event.timestamp_ = timestamp;
  event.mTmsi_ = mTmsi;
  event.enodebID_ = enodebID;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

template <IsCgiCompatible CgiArg>
Event Event::CreateIdentityResponse(const unsigned long timestamp,
                                    const unsigned long imsi,
                                    const unsigned int enodebID,
                                    const unsigned int mmeID,
                                    CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::IdentityResponse;
  event.timestamp_ = timestamp;
  event.imsi_ = imsi;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

Event Event::CreateAttachAccept(const unsigned long timestamp,
                                const unsigned int enodebID,
                                const unsigned int mmeID,
                                const unsigned int mTmsi)
{
  Event event{};

  event.type_ = Type::AttachAccept;
  event.timestamp_ = timestamp;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;
  event.mTmsi_ = mTmsi;

  return event;
}

template <IsCgiCompatible CgiArg>
Event Event::CreatePaging(const unsigned long timestamp,
                          const unsigned int mTmsi,
                          CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::Paging;
  event.timestamp_ = timestamp;
  event.mTmsi_ = mTmsi;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

template <IsCgiCompatible CgiArg>
Event Event::CreatePathSwitchRequest(const unsigned long timestamp,
                                     const unsigned int enodebID,
                                     const unsigned int mmeID,
                                     CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::PathSwitchRequest;
  event.timestamp_ = timestamp;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

Event Event::CreatePathSwitchRequestAcknowledge(const unsigned long timestamp,
                                                const unsigned int enodebID,
                                                const unsigned int mmeID)
{
  Event event{};

  event.type_ = Type::PathSwitchRequestAcknowledge;
  event.timestamp_ = timestamp;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;

  return event;
}

template <IsCgiCompatible CgiArg>
Event Event::CreateUEContextReleaseCommand(const unsigned long timestamp,
                                           const unsigned int enodebID,
                                           const unsigned int mmeID,
                                           CgiArg&& cgi)
{
  Event event{};

  event.type_ = Type::UEContextReleaseCommand;
  event.timestamp_ = timestamp;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;
  event.cgi_ = std::forward<CgiArg>(cgi);

  return event;
}

Event Event::CreateUEContextReleaseResponse(const unsigned long timestamp,
                                            const unsigned int enodebID,
                                            const unsigned int mmeID)
{
  Event event{};

  event.type_ = Type::UEContextReleaseResponse;
  event.timestamp_ = timestamp;
  event.enodebID_ = enodebID;
  event.mmeID_ = mmeID;

  return event;
}

std::expected<void, Event::Error> Event::Verify() const
{
  switch (type_)
  {
    case Type::AttachRequest:
      return VerifyAttachRequest();

    case Type::IdentityResponse:
      return VerifyIdentityResponse();

    case Type::AttachAccept:
      return VerifyAttachAccept();

    case Type::Paging:
      return VerifyPaging();

    case Type::PathSwitchRequest:
      return VerifyPathSwitchRequest();

    case Type::PathSwitchRequestAcknowledge:
      return VerifyPathSwitchRequestAcknowledge();

    case Type::UEContextReleaseCommand:
      return VerifyUEContextReleaseCommand();

    case Type::UEContextReleaseResponse:
      return VerifyUEContextReleaseResponse();

    default:
      return std::unexpected(Error::WrongEventType);
  }
}

std::expected<void, Event::Error> Event::VerifyAttachRequest() const
{
  if (imsi_.has_value() && mTmsi_.has_value())
    return std::unexpected(Error::WrongImsiAndMTmsiArgs);
  if (!imsi_.has_value() && !mTmsi_.has_value())
    return std::unexpected(Error::MissingImsiOrMTmsi);
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!cgi_.has_value())
    return std::unexpected(Error::BadCgi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyIdentityResponse() const
{
  if (!imsi_.has_value())
    return std::unexpected(Error::ImsiNotExist);
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  if (!cgi_.has_value())
    return std::unexpected(Error::BadCgi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyAttachAccept() const
{
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  if (!mTmsi_.has_value())
    return std::unexpected(Error::BadMTmsi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyPaging() const
{
  if (!mTmsi_.has_value())
    return std::unexpected(Error::BadMTmsi);
  if (!cgi_.has_value())
    return std::unexpected(Error::BadCgi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyPathSwitchRequest() const
{
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  if (!cgi_.has_value())
    return std::unexpected(Error::BadCgi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyPathSwitchRequestAcknowledge() const
{
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  return {};
}

std::expected<void, Event::Error> Event::VerifyUEContextReleaseCommand() const
{
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  if (!cgi_.has_value())
    return std::unexpected(Error::BadCgi);
  return {};
}

std::expected<void, Event::Error> Event::VerifyUEContextReleaseResponse() const
{
  if (!enodebID_.has_value())
    return std::unexpected(Error::BadEnodebID);
  if (!mmeID_.has_value())
    return std::unexpected(Error::BadMmeID);
  return {};
}

S1apOut::Type S1apOut::GetType() const { return type_; }
unsigned long S1apOut::GetImsi() const { return imsi_; }
const std::optional<std::vector<unsigned char>>& S1apOut::GetCgi() const { return cgi_; }

void S1apDB::Subscriber::SetLastEvent(const Event::Type eventType, const unsigned long timestamp)
{
  eventType_ = eventType;
  lastEventTimestamp_ = timestamp;
}

void S1apDB::Subscriber::SetMTmsi(const unsigned int mTmsi) { mTmsi_ = mTmsi; }
void S1apDB::Subscriber::SetEnodebID(const unsigned int enodebID) { enodebID_ = enodebID; }
void S1apDB::Subscriber::SetMmeID(const unsigned int mmeID) { mmeID_ = mmeID; }
void S1apDB::Subscriber::SetState(const State state) { state_ = state; }

std::optional<unsigned long> S1apDB::Subscriber::GetImsi() const { return imsi_; }
std::optional<unsigned int> S1apDB::Subscriber::GetMTmsi() const { return mTmsi_; }
std::optional<unsigned int> S1apDB::Subscriber::GetEnodebID() const { return enodebID_; }
std::optional<unsigned int> S1apDB::Subscriber::GetMmeID() const { return mmeID_; }

const std::optional<std::vector<unsigned char>>& S1apDB::Subscriber::GetCgi() const { return cgi_; }

Event::Type S1apDB::Subscriber::GetLastEventType() const { return eventType_; }
unsigned long S1apDB::Subscriber::GetLastEventTimestamp() const { return lastEventTimestamp_; }
S1apDB::Subscriber::State S1apDB::Subscriber::GetState() const { return state_; }

S1apDB& S1apDB::GetInstance()
{
  static S1apDB s1apDB{};
  return s1apDB;
}

unsigned int S1apDB::GenerateNewMTmsi()
{
  return nextMTmsi_++;
}

std::expected<unsigned long, S1apDB::HandleError> S1apDB::ResolveImsi(const Event& event)
{
  if (event.GetImsi().has_value())
    return event.GetImsi().value();

  else if (event.GetMTmsi().has_value())
  {
    auto it = mTmsiToImsi.find(event.GetMTmsi().value());
    if (it != mTmsiToImsi.end())
      return it->second;
  }

  return std::unexpected(Error::NoImsiOrMTmsiInEvent);
}

S1apDB::HandleOut S1apDB::Handle(const Event& event)
{
  auto verifyResult = event.Verify();

  if (!verifyResult.has_value())
    return std::unexpected(verifyResult.error());

  switch (event.GetType())
  {
    case Event::Type::AttachRequest:
      return HandleAttachRequest(event);

    case Event::Type::IdentityResponse:
      return HandleIdentityResponse(event);

    case Event::Type::Paging:
      return HandlePaging(event);

    case Event::Type::PathSwitchRequest:
      return HandlePathSwitchRequest(event);

    case Event::Type::UEContextReleaseResponse:
      return HandleUEContextReleaseResponse(event);

    case Event::Type::AttachAccept:
      return HandleAttachAccept(event);

    case Event::Type::PathSwitchRequestAcknowledge:
      return HandlePathSwitchRequestAcknowledge(event);

    case Event::Type::UEContextReleaseCommand:
      return HandleUEContextReleaseCommand(event);

    default:
      return std::unexpected(Event::Error::WrongEventType);
  }
}

S1apDB::HandleOut S1apDB::HandleAttachRequest(const Event& event)
{
    auto imsiResult = ResolveImsiFromEvent(event);
    if (!imsiResult.has_value()) {
        if (event.GetMTmsi().has_value())
        {
             std::println("MME: Received Attach Request with unknown MTmsi: {}. Sending Identity Request.", event.GetMTmsi().value());
             return std::nullopt;
        }

        return std::unexpected(Error::NoImsiOrMTmsiInEvent);
    }

    const auto imsi = imsiResult.value();
    auto it = imsiToSubscriber.find(imsi);

    if (it == imsiToSubscriber.end())
        return ProcessNewAttach(event);

    Subscriber& subscriber = it->second;

    if (subscriber.GetState() == Subscriber::State::ATTACHED)
        return ProcessDuplicateAttach(subscriber, event);

    return ProcessExistingAttach(subscriber, event);
}

S1apDB::HandleOut S1apDB::HandleIdentityResponse(const Event& event)
{
    unsigned long imsi = event.GetImsi().value();
    auto it = imsiToSubscriber.find(imsi);

    if (it == imsiToSubscriber.end())
        return ProcessIdentityResponseForNewUser(event);

    Subscriber& subscriber = it->second;

    if (subscriber.GetState() == Subscriber::State::ATTACHING)
        return ProcessIdentityResponseForAttachingUser(subscriber, event);

    std::println(stderr, "MME: Received Identity Response for user {} in unexpected state: {}. Ignoring.",
                 imsi, static_cast<int>(subscriber.GetState()));

    return std::nullopt;
}

S1apDB::HandleOut S1apDB::HandlePaging(const Event& event)
{
    return ResolveImsiFromEvent(event)
        .and_then([&](unsigned long imsi) -> HandleOut {
            auto it = imsiToSubscriber.find(imsi);
            if (it == imsiToSubscriber.end()) {
                std::println(stderr, "MME: Paging for non-existent subscriber IMSI: {} (from MTmsi: {})", imsi, event.GetMTmsi().value());
                return std::unexpected(Error::SubscriberNotFound);
            }
            return ProcessPagingRequest(it->second, event);
        });
}

S1apDB::HandleOut S1apDB::HandlePathSwitchRequest(const Event& event)
{
    unsigned int oldEnodebID = event.GetEnodebID().value();
    
    return ResolveImsiFromEnodebID(oldEnodebID)
        .and_then([&](unsigned long imsi) -> HandleOut {
            auto it = imsiToSubscriber.find(imsi);
            if (it == imsiToSubscriber.end()) {
                std::println(stderr, "MME: Path Switch Request for non-existent subscriber IMSI: {}", imsi);
                return std::unexpected(Error::SubscriberNotFound);
            }
            return ProcessPathSwitchRequest(it->second, event);
        });
}

S1apDB::HandleOut S1apDB::HandleUEContextReleaseResponse(const Event& event)
{
    unsigned int enodebID = event.GetEnodebID().value();
    
    return ResolveImsiFromEnodebID(enodebID)
        .and_then([&](unsigned long imsi) -> HandleOut {
            auto it = imsiToSubscriber.find(imsi);
            if (it == imsiToSubscriber.end()) {
                std::println(stderr, "MME: UE Context Release Response for non-existent subscriber IMSI: {}", imsi);
                return std::unexpected(Error::SubscriberNotFound);
            }
            return ProcessUEContextRelease(it->second, event);
        });
}

