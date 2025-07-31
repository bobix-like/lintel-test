#include "gtest/gtest.h"
#include "S1apDB.hpp"

TEST(EventTest, GettersReturnCorrectValues) {
    S1ap::Timestamp timestamp = 12345;
    S1ap::Imsi imsi = 987654321;
    S1ap::EnodebID enodebID = 100;
    S1ap::MmeID mmeID = 200;
    S1ap::Cgi cgi = {0x01, 0x02, 0x03};

    Event event = Event::CreateIdentityResponse(timestamp, imsi, enodebID, mmeID, cgi);

    ASSERT_EQ(event.GetType(), Event::Type::IdentityResponse);
    ASSERT_EQ(event.GetTimestamp(), timestamp);
    ASSERT_TRUE(event.GetImsi().has_value());
    ASSERT_EQ(event.GetImsi().value(), imsi);
    ASSERT_TRUE(event.GetEnodebID().has_value());
    ASSERT_EQ(event.GetEnodebID().value(), enodebID);
    ASSERT_TRUE(event.GetMmeID().has_value());
    ASSERT_EQ(event.GetMmeID().value(), mmeID);
    ASSERT_TRUE(event.GetCgi().has_value());
    ASSERT_EQ(event.GetCgi().value(), cgi);
    ASSERT_FALSE(event.GetMTmsi().has_value());
}

TEST(EventTest, VerifyAttachRequestWithImsi) {
    S1ap::Timestamp timestamp = 123;
    S1ap::Imsi imsi = 12345;
    S1ap::EnodebID enodebID = 1;
    S1ap::Cgi cgi = {0x01, 0x02, 0x03};

    Event event = Event::CreateAttachRequestWithImsi(timestamp, imsi, enodebID, cgi);
    auto result = event.Verify();

    ASSERT_TRUE(result.has_value());
}

TEST(EventTest, VerifyAttachRequestFailsWithBothImsiAndMTmsi) {
    S1ap::Timestamp timestamp = 123;
    S1ap::Imsi imsi = 12345;
    S1ap::EnodebID enodebID = 1;
    S1ap::Cgi cgi = {0x01, 0x02, 0x03};

    Event event = Event::CreateAttachRequestWithImsi(timestamp, imsi, enodebID, cgi);

    const_cast<S1ap::OMTmsi&>(event.GetMTmsi()).emplace(5000); 

    auto result = event.Verify();

    ASSERT_FALSE(result.has_value());
    ASSERT_EQ(result.error(), Event::Error::WrongImsiAndMTmsiArgs);
}

TEST(S1apOutTest, S1apOutConstructorWorks) {
    S1ap::Imsi imsi = 12345;
    S1ap::Cgi cgi = {0x10, 0x20};

    S1apOut out(S1apOut::Type::Reg, imsi, cgi);

    ASSERT_EQ(out.GetType(), S1apOut::Type::Reg);
    ASSERT_EQ(out.GetImsi(), imsi);
    ASSERT_TRUE(out.GetCgi().has_value());
    ASSERT_EQ(out.GetCgi().value(), cgi);
}

TEST(S1apDBTest, HandleAttachRequest) {
    S1apDB& db = S1apDB::GetInstance();
    S1ap::Imsi imsi = 123456789;
    S1ap::EnodebID enodebID = 1000;
    S1ap::Timestamp timestamp = 10000;
    S1ap::Cgi cgi = {0x01, 0x02, 0x03};

    Event attachRequest = Event::CreateAttachRequestWithImsi(timestamp, imsi, enodebID, cgi);
    auto result = db.Handle(attachRequest);

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result.value().has_value());
    ASSERT_EQ(result.value().value().GetType(), S1apOut::Type::Reg);
    ASSERT_EQ(result.value().value().GetImsi(), imsi);
}
