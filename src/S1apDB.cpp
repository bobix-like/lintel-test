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

S1apDB::HandleOut S1apDB::ProcessNewAttach(const Event& event)
{
  unsigned long imsi = event.GetImsi().value();

  Subscriber newSubscriber;

  newSubscriber.SetImsi(imsi);
  newSubscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  newSubscriber.SetState(Subscriber::State::ATTACHED);
  newSubscriber.SetEnodebID(event.GetEnodebID().value());

  if (event.GetCgi().has_value())
      newSubscriber.SetCgi(event.GetCgi().value());

  imsiToSubscriber[imsi] = newSubscriber;

  unsigned int newMTmsi = GenerateNewMTmsi();
  imsiToSubscriber[imsi].SetMTmsi(newMTmsi);
  mTmsiToImsi[newMTmsi] = imsi;
  enodebIDToImsi[newSubscriber.GetEnodebID().value()] = imsi;

  std::println("MME: User {} attached. Assigned MTmsi: {}", imsi, newMTmsi);

  return S1apOut(S1apOut::Type::Reg, imsi, event.GetCgi());
}

S1apDB::HandleOut S1apDB::ProcessExistingAttach(Subscriber& subscriber, const Event& event)
{
  subscriber.SetState(Subscriber::State::ATTACHED);
  subscriber.SetEnodebID(event.GetEnodebID().value());

  if (event.GetCgi().has_value())
      subscriber.SetCgi(event.GetCgi().value());

  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());

  const auto currentMTmsi = subscriber.GetMTmsi().value_or(GenerateNewMTmsi());

  if (!subscriber.GetMTmsi().has_value())
  {
      subscriber.SetMTmsi(currentMTmsi);
      mTmsiToImsi[currentMTmsi] = event.GetImsi().value();
  }

  enodebIDToImsi[event.GetEnodebID().value()] = subscriber.GetImsi().value();
  std::println("MME: User {} re-attached. Current MTmsi: {}", event.GetImsi().value(), currentMTmsi);

  return S1apOut(S1apOut::Type::Reg, event.GetImsi().value(), event.GetCgi());
}

S1apDB::HandleOut S1apDB::ProcessDuplicateAttach(Subscriber& subscriber, const Event& event)
{
  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  std::println("MME: User {} already attached. Ignoring duplicate Attach Request.", event.GetImsi().value());

  return std::nullopt;
}

S1apDB::HandleOut S1apDB::ProcessIdentityResponseForNewUser(const Event& event)
{
  unsigned long imsi = event.GetImsi().value();

  imsiToSubscriber[imsi] = {};
  auto& newSubscriber = imsiToSubscriber[imsi];

  newSubscriber.SetImsi(imsi);
  newSubscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  newSubscriber.SetState(Subscriber::State::ATTACHED);
  newSubscriber.SetEnodebID(event.GetEnodebID().value());

  if (event.GetCgi().has_value())
    newSubscriber.SetCgi(event.GetCgi().value());

  unsigned int newMTmsi = GenerateNewMTmsi();
  imsiToSubscriber[imsi].SetMTmsi(newMTmsi);
  mTmsiToImsi[newMTmsi] = imsi;
  enodebIDToImsi[event.GetEnodebID().value()] = imsi;
  imsiToIdentityRequestTimeout_.erase(imsi);

  std::println("MME: Received Identity Response for user {}. User attached. Assigned MTmsi: {}", imsi, newMTmsi);
  return S1apOut(S1apOut::Type::Reg, imsi, event.GetCgi());
}

S1apDB::HandleOut S1apDB::ProcessIdentityResponseForAttachingUser(Subscriber& subscriber, const Event& event)
{
  subscriber.SetState(Subscriber::State::ATTACHED);
  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  subscriber.SetEnodebID(event.GetEnodebID().value());

  if (event.GetCgi().has_value())
    subscriber.SetCgi(event.GetCgi().value());

  unsigned int currentMTmsi = subscriber.GetMTmsi().value_or(GenerateNewMTmsi());

  if (!subscriber.GetMTmsi().has_value())
  {
      subscriber.SetMTmsi(currentMTmsi);
      mTmsiToImsi[currentMTmsi] = event.GetImsi().value();
  }

  enodebIDToImsi[event.GetEnodebID().value()] = subscriber.GetImsi().value();
  imsiToIdentityRequestTimeout_.erase(event.GetImsi().value());

  std::println("MME: User {} moved from ATTACHING to ATTACHED. Current MTmsi: {}", event.GetImsi().value(), currentMTmsi);
  return S1apOut(S1apOut::Type::Reg, event.GetImsi().value(), event.GetCgi());
}

S1apDB::HandleOut S1apDB::ProcessPagingRequest(Subscriber& subscriber, const Event& event)
{
  if (subscriber.GetState() != Subscriber::State::ATTACHED
  &&  subscriber.GetState() != Subscriber::State::DETACHED)
  {
      std::println(stderr, "MME: Paging for user {} received in unexpected state: {}. Ignoring.",
                   subscriber.GetImsi().value(), static_cast<int>(subscriber.GetState()));

      return std::nullopt;
  }

  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  subscriber.SetState(Subscriber::State::PAGING_STATE);

  std::println("MME: Paging for user {} (MTmsi: {}). Changing state to PAGING_STATE.",
               subscriber.GetImsi().value(), event.GetMTmsi().value());

  return std::nullopt;
}

S1apDB::HandleOut S1apDB::ProcessPathSwitchRequest(Subscriber& subscriber, const Event& event)
{
  if (subscriber.GetState() != Subscriber::State::ATTACHED)
  {
      std::println(stderr, "MME: Path Switch Request for user {} received in unexpected state: {}. Ignoring.",
                   subscriber.GetImsi().value(), static_cast<int>(subscriber.GetState()));

      return std::unexpected(Error::WrongState);
  }

  unsigned int oldEnodebID = event.GetEnodebID().value();
  unsigned int newEnodebID = event.GetCgi().value_or(std::vector<unsigned char>{}).front();

  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());
  subscriber.SetEnodebID(newEnodebID);
  subscriber.SetState(Subscriber::State::HANDOVER_STATE);

  enodebIDToImsi.erase(oldEnodebID);
  enodebIDToImsi[newEnodebID] = subscriber.GetImsi().value();

  std::println("MME: Path Switch Request for user {}. Moved from eNodeB {} to {}.",
               subscriber.GetImsi().value(), oldEnodebID, newEnodebID);

  return S1apOut(S1apOut::Type::CgiChange, subscriber.GetImsi().value(), event.GetCgi());
}

S1apDB::HandleOut S1apDB::ProcessUEContextRelease(Subscriber& subscriber, const Event& event)
{
  unsigned long imsi = subscriber.GetImsi().value();
  
  subscriber.SetState(Subscriber::State::DETACHED);
  subscriber.SetLastEvent(event.GetType(), event.GetTimestamp());

  DetachSubscriber(subscriber);
  
  std::println("MME: UE Context for user {} released. User detached.", imsi);
  return S1apOut(S1apOut::Type::UnReg, imsi, subscriber.GetCgi());
}

void S1apDB::DetachSubscriber(Subscriber& subscriber)
{
  if (subscriber.GetMTmsi().has_value())
      mTmsiToImsi.erase(subscriber.GetMTmsi().value());

  if (subscriber.GetEnodebID().has_value())
      enodebIDToImsi.erase(subscriber.GetEnodebID().value());

  imsiToSubscriber.erase(subscriber.GetImsi().value());
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

