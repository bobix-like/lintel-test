#include "S1apDB.hpp"
#include <optional>

S1apDB& S1apDB::GetInstance()
{
  static S1apDB s1apDB{};
  return s1apDB;
}

S1apDB::HandleOut S1apDB::Handle(const Event& event)
{
  auto verifyResult = event.Verify();
  if (!verifyResult.has_value())
  {
    return std::unexpected(verifyResult.error());
  }

  switch (event.GetType())
  {
    case Event::Type::AttachRequest:
      if (event.GetImsi() != std::nullopt)
      {
        return S1apDB::HandleAttachRequestWithImsi(event);
      }
      else
      {
        return S1apDB::HandleAttachRequestWithMTmsi(event);
      }

    case Event::Type::IdentityResponse:
      return S1apDB::HandleIdentityResponse(event);

    case Event::Type::AttachAccept:
      return S1apDB::HandleAttachAccept(event);

    case Event::Type::Paging:
      return S1apDB::HandlePaging(event);

    case Event::Type::PathSwitchRequest:
      return S1apDB::HandlePathSwitchRequest(event);

    case Event::Type::PathSwitchRequestAcknowledge:
      return S1apDB::HandlePathSwitchRequestAcknowledge(event);

    case Event::Type::UEContextReleaseCommand:
      return S1apDB::HandleUEContextReleaseCommand(event);

    case Event::Type::UEContextReleaseResponse:
      return S1apDB::HandleUEContextReleaseResponse(event);

    default:
      return std::unexpected(Event::Error::WrongEventType);
  }
}

S1apDB::HandleOut HandleAttachRequestWithImsi(const Event& event)
{

}

S1apDB::HandleOut HandleAttachRequestWithMTmsi(const Event& event)
{

}

S1apDB::HandleOut HandleIdentityResponse(const Event& event)
{

}

S1apDB::HandleOut HandleAttachAccept(const Event& event)
{

}

S1apDB::HandleOut HandlePaging(const Event& event)
{

}

S1apDB::HandleOut HandlePathSwitchRequest(const Event& event)
{

}

S1apDB::HandleOut HandlePathSwitchRequestAcknowledge(const Event& event)
{

}

S1apDB::HandleOut HandleUEContextReleaseCommand(const Event& event)
{

}

S1apDB::HandleOut HandleUEContextReleaseResponse(const Event& event)
{

}

