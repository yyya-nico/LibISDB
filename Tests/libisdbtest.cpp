/*
  LibISDB
  Copyright(c) 2017-2020 DBCTRADO

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 @file   libisdbtest.cpp
 @brief  ユニットテスト
 @author DBCTRADO
*/


#include "../LibISDB/LibISDB.hpp"

#include <clocale>

#if defined(LIBISDB_WINDOWS) && defined(LIBISDB_WCHAR)
#define LIBISDB_TEST_WMAIN
#endif

#ifndef LIBISDB_TEST_WMAIN
#define CATCH_CONFIG_MAIN
#else
#define CATCH_CONFIG_RUNNER
#endif
#include "../Thirdparty/Catch/catch.hpp"

#include "../LibISDB/Base/DebugDef.hpp"


using namespace LibISDB::Literals;

namespace
{
#ifdef __cpp_char8_t
	typedef char8_t Char8;
#else
	typedef char Char8;
#endif

	const uint8_t * operator"" _b8(const char *str, size_t length) {
		return reinterpret_cast<const uint8_t *>(str);
	}

#ifdef __cpp_char8_t
	const uint8_t * operator"" _b8(const char8_t *str, size_t length) {
		return reinterpret_cast<const uint8_t *>(str);
	}
#endif
}


#include "../LibISDB/Utilities/AlignedAlloc.hpp"

TEST_CASE("AlignedAlloc", "[utility][memory]")
{
	std::uint8_t *buffer = static_cast<std::uint8_t *>(LibISDB::AlignedAlloc(32, 16));
	CHECK((reinterpret_cast<std::uintptr_t>(buffer) & 15) == 0);
	for (size_t i = 0; i < 32; i++)
		buffer[i] = static_cast<std::uint8_t>(i);
	buffer = static_cast<std::uint8_t *>(LibISDB::AlignedRealloc(buffer, 64, 32));
	CHECK((reinterpret_cast<std::uintptr_t>(buffer) & 31) == 0);
	CHECK(
		[buffer]() -> bool {
			for (size_t i = 0; i < 32; i++) {
				if (buffer[i] != i)
					return false;
			}
			return true;
		}());
	buffer = static_cast<std::uint8_t *>(LibISDB::AlignedRealloc(buffer, 16, 32, 5));
	CHECK(((reinterpret_cast<std::uintptr_t>(buffer) + 5) & 31) == 0);
	CHECK(
		[buffer]() -> bool {
			for (size_t i = 0; i < 16; i++) {
				if (buffer[i] != i)
					return false;
			}
			return true;
		}());
	LibISDB::AlignedFree(buffer);

	buffer = static_cast<std::uint8_t *>(LibISDB::AlignedAlloc(16, 16, 15));
	CHECK(((reinterpret_cast<std::uintptr_t>(buffer) + 15) & 15) == 0);
	buffer = static_cast<std::uint8_t *>(LibISDB::AlignedRealloc(buffer, 64, 32, 2));
	CHECK(((reinterpret_cast<std::uintptr_t>(buffer) + 2) & 31) == 0);
	LibISDB::AlignedFree(buffer);
}


#include "../LibISDB/Utilities/CRC.hpp"

TEST_CASE("CRC", "[utility][hash]")
{
	// CRC-16 (IBM)
	{
		CHECK(LibISDB::CRC16::Calc(nullptr, 0) == LibISDB::CRC16::InitialValue);
		CHECK(LibISDB::CRC16::Calc(u8"The quick brown fox jumps over the lazy dog"_b8, 43) == 0x60AE_u16);

		LibISDB::Hasher<LibISDB::CRC16> crc16;

		CHECK(crc16.Get() == LibISDB::CRC16::InitialValue);
		CHECK(crc16.Calc(u8"The quick brown fox "_b8, 20) == 0xD2A3_u16);
		CHECK(crc16.Calc(u8"jumps over the lazy dog"_b8, 23) == 0x60AE_u32);
		CHECK(crc16.Get() == 0x60AE_u32);
	}

	// CRC-16-CCITT
	{
		CHECK(LibISDB::CRC16CCITT::Calc(nullptr, 0) == LibISDB::CRC16CCITT::InitialValue);
		CHECK(LibISDB::CRC16CCITT::Calc(u8"The quick brown fox jumps over the lazy dog"_b8, 43) == 0xF0C8_u16);

		LibISDB::Hasher<LibISDB::CRC16CCITT> crc16;

		CHECK(crc16.Get() == LibISDB::CRC16::InitialValue);
		CHECK(crc16.Calc(u8"The quick brown fox "_b8, 20) == 0x9D25_u16);
		CHECK(crc16.Calc(u8"jumps over the lazy dog"_b8, 23) == 0xF0C8_u16);
		CHECK(crc16.Get() == 0xF0C8_u16);
	}

	// CRC-32
	{
		CHECK(LibISDB::CRC32::Calc(nullptr, 0) == LibISDB::CRC32::InitialValue);
		CHECK(LibISDB::CRC32::Calc(u8"The quick brown fox jumps over the lazy dog"_b8, 43) == 0x414FA339_u32);

		LibISDB::Hasher<LibISDB::CRC32> crc32;

		CHECK(crc32.Get() == LibISDB::CRC32::InitialValue);
		CHECK(crc32.Calc(u8"The quick brown fox "_b8, 20) == 0x88B075E2_u32);
		CHECK(crc32.Calc(u8"jumps over the lazy dog"_b8, 23) == 0x414FA339_u32);
		CHECK(crc32.Get() == 0x414FA339_u32);
	}

	// CRC-32/MPEG-2
	{
		CHECK(LibISDB::CRC32MPEG2::Calc(nullptr, 0) == LibISDB::CRC32MPEG2::InitialValue);
		CHECK(LibISDB::CRC32MPEG2::Calc(u8"The quick brown fox jumps over the lazy dog"_b8, 43) == 0xBA62119E_u32);

		LibISDB::Hasher<LibISDB::CRC32MPEG2> crc32;

		CHECK(crc32.Get() == LibISDB::CRC32MPEG2::InitialValue);
		CHECK(crc32.Calc(u8"The quick brown fox "_b8, 20) == 0xF7E3F54F_u32);
		CHECK(crc32.Calc(u8"jumps over the lazy dog"_b8, 23) == 0xBA62119E_u32);
		CHECK(crc32.Get() == 0xBA62119E_u32);
	}
}


#include "../LibISDB/Utilities/Sort.hpp"

TEST_CASE("Sort", "[utility][sort]")
{
	int list1[] = {6, 3, 1, 2, 4, 0, 5};
	LibISDB::InsertionSort(list1);
	CHECK(
		[&list1]() -> bool {
			for (int i = 0; i < static_cast<int>(std::size(list1)); i++) {
				if (list1[i] != i)
					return false;
			}
			return true;
		}());
	LibISDB::InsertionSort(list1, list1 + std::size(list1), std::greater<int>());
	CHECK(
		[&list1]() -> bool {
			for (int i = 0; i < static_cast<int>(std::size(list1)); i++) {
				if (list1[i] != static_cast<int>(std::size(list1)) - 1 - i)
					return false;
			}
			return true;
		}());

	struct item {
		int value;
		LibISDB::String text;
		bool operator < (const item &rhs) const { return value < rhs.value; }
	};
	item list2[] = {
		{3, LIBISDB_STR("three-1")},
		{2, LIBISDB_STR("two-1")},
		{3, LIBISDB_STR("three-2")},
		{1, LIBISDB_STR("one")},
		{0, LIBISDB_STR("zero")},
		{2, LIBISDB_STR("two-2")},
	};
	const item list2_sorted[] = {
		{0, LIBISDB_STR("zero")},
		{1, LIBISDB_STR("one")},
		{2, LIBISDB_STR("two-1")},
		{2, LIBISDB_STR("two-2")},
		{3, LIBISDB_STR("three-1")},
		{3, LIBISDB_STR("three-2")},
	};
	LibISDB::InsertionSort(list2);
	CHECK((
		[&list2, &list2_sorted]() -> bool {
			for (int i = 0; i < static_cast<int>(std::size(list2)); i++) {
				if ((list2[i].value != list2_sorted[i].value)
						|| (list2[i].text != list2_sorted[i].text))
					return false;
			}
			return true;
		}()));
}


#include "../LibISDB/Utilities/MD5.hpp"

TEST_CASE("MD5", "[utility][hash]")
{
	LibISDB::MD5Value md5;

	static const LibISDB::MD5Value zero_md5 = {
		0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04, 0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E
	};
	md5 = LibISDB::CalcMD5(nullptr, 0);
	CHECK(md5 == zero_md5);

	static const LibISDB::MD5Value fox_md5 = {
		0x9E, 0x10, 0x7D, 0x9D, 0x37, 0x2B, 0xB6, 0x82, 0x6B, 0xD8, 0x1D, 0x35, 0x42, 0xA4, 0x19, 0xD6
	};
	md5 = LibISDB::CalcMD5(u8"The quick brown fox jumps over the lazy dog"_b8, 43);
	CHECK(md5 == fox_md5);
}


#include "../LibISDB/Utilities/Utilities.hpp"

TEST_CASE("Load", "[utility][load]")
{
	const Char8 *Data = u8"ABCDE";

	CHECK(LibISDB::Load16(Data) == 0x4142_u16);
	CHECK(LibISDB::Load16(Data + 1) == 0x4243_u16);
	CHECK(LibISDB::Load24(Data) == 0x414243_u32);
	CHECK(LibISDB::Load24(Data + 1) == 0x424344_u32);
	CHECK(LibISDB::Load32(Data) == 0x41424344_u32);
	CHECK(LibISDB::Load32(Data + 1) == 0x42434445_u32);
}


#include "../LibISDB/Base/ARIBString.hpp"

TEST_CASE("ARIBString", "[base][string]")
{
	LibISDB::ARIBStringDecoder decoder;
	LibISDB::String str;

	// "テレビショッピング"
	decoder.Decode("\x1b\x7c\xc6\xec\xd3\xb7\xe7\xc3\xd4\xf3\xb0"_b8, 11, &str);
	CHECK(str.compare(LIBISDB_STR("\u30c6\u30ec\u30d3\u30b7\u30e7\u30c3\u30d4\u30f3\u30b0")) == 0);

	// "番組内容②"
	decoder.Decode("\x48\x56\x41\x48\x46\x62\x4d\x46\x1b\x24\x2a\x3b\x1b\x7d\xfe\xe2"_b8, 16, &str);
	CHECK(str.compare(LIBISDB_STR("\u756a\u7d44\u5185\u5bb9\u2461")) == 0);
}


#include "../LibISDB/Base/DateTime.hpp"

TEST_CASE("DateTime", "[base][time]")
{
	LibISDB::DateTime Time;

	CHECK_FALSE(Time.IsValid());

	Time.Year        = 2000;
	Time.Month       = 12;
	Time.Day         = 31;
	Time.Hour        = 23;
	Time.Minute      = 59;
	Time.Second      = 59;
	Time.Millisecond = 0;

	Time.SetDayOfWeek();
	CHECK(Time.DayOfWeek == 0);

	CHECK(Time.IsValid());

	Time.OffsetSeconds(1);

	CHECK(Time.Year == 2001);
	CHECK(Time.Month == 1);
	CHECK(Time.Day == 1);
	CHECK(Time.DayOfWeek == 1);
	CHECK(Time.Hour == 0);
	CHECK(Time.Minute == 0);
	CHECK(Time.Second == 0);
	CHECK(Time.Millisecond == 0);

	LibISDB::DateTime Time2(Time);

	CHECK(Time2.IsValid());
	CHECK(Time2 == Time);
	CHECK_FALSE(Time2 < Time);
	CHECK(Time2 <= Time);
	CHECK_FALSE(Time2 > Time);
	CHECK(Time2 >= Time);
	CHECK(Time2.Compare(Time) == 0);
	CHECK(Time2.DiffMilliseconds(Time) == 0LL);

	Time2.OffsetMinutes(-30);

	CHECK(Time2.Year == 2000);
	CHECK(Time2.Month == 12);
	CHECK(Time2.Day == 31);
	CHECK(Time2.DayOfWeek == 0);
	CHECK(Time2.Hour == 23);
	CHECK(Time2.Minute == 30);
	CHECK(Time2.Second == 0);
	CHECK(Time2.Millisecond == 0);

	CHECK(Time2 != Time);
	CHECK(Time2 < Time);
	CHECK(Time2 <= Time);
	CHECK(Time > Time2);
	CHECK(Time >= Time2);
	CHECK(Time2.Compare(Time) < 0);
	CHECK(Time.Compare(Time2) > 0);
	CHECK(Time2.Diff(Time) == std::chrono::milliseconds(-30LL * 60LL * 1000LL));
	CHECK(Time2.DiffMilliseconds(Time) == -30LL * 60LL * 1000LL);
	CHECK(Time2.DiffSeconds(Time) == -30LL * 60LL);

	unsigned long long LinearSeconds = Time.GetLinearSeconds();
	Time2.FromLinearSeconds(LinearSeconds);
	CHECK(Time == Time2);

	LinearSeconds += 60;
	Time.FromLinearSeconds(LinearSeconds);
	Time2.OffsetSeconds(60);
	CHECK(Time == Time2);

	Time.Millisecond = 500;
	unsigned long long LinearMilliseconds = Time.GetLinearMilliseconds();
	CHECK(LinearMilliseconds == LinearSeconds * 1000ULL + 500ULL);
	Time2.FromLinearMilliseconds(LinearMilliseconds);
	CHECK(Time == Time2);
}


#include "../LibISDB/TS/Tables.hpp"
#include "../LibISDB/TS/TSPacket.hpp"
#include "../LibISDB/Base/ARIBTime.hpp"
#include "../LibISDB/Filters/AnalyzerFilter.hpp"

namespace
{
	void Append16(std::vector<uint8_t> *pData, uint16_t Value)
	{
		pData->push_back(static_cast<uint8_t>(Value >> 8));
		pData->push_back(static_cast<uint8_t>(Value));
	}

	void AppendCRC32MPEG2(std::vector<uint8_t> *pSection)
	{
		const size_t SectionLength = pSection->size() - 3 + 4;

		REQUIRE(SectionLength <= 0x0FFF);
		(*pSection)[1] = static_cast<uint8_t>(((*pSection)[1] & 0xF0) | (SectionLength >> 8));
		(*pSection)[2] = static_cast<uint8_t>(SectionLength);

		const uint32_t CRC = LibISDB::CRC32MPEG2::Calc(pSection->data(), pSection->size());
		pSection->push_back(static_cast<uint8_t>(CRC >> 24));
		pSection->push_back(static_cast<uint8_t>(CRC >> 16));
		pSection->push_back(static_cast<uint8_t>(CRC >> 8));
		pSection->push_back(static_cast<uint8_t>(CRC));

		REQUIRE(LibISDB::CRC32MPEG2::Calc(pSection->data(), pSection->size()) == 0);
	}

	enum class SITSyntax {
		Standard,
		Extended,
	};

	std::vector<uint8_t> MakeSITSection(
		bool InvalidTransmissionInfoLength = false,
		SITSyntax Syntax = SITSyntax::Standard)
	{
		std::vector<uint8_t> Section = {
			0x7F,       // table_id
			static_cast<uint8_t>(Syntax == SITSyntax::Extended ? 0xF0 : 0x70), 0x00,
			0xFF, 0xFF, // reserved_future_use
			0xC7,       // ISO_reserved, version_number = 3, current_next_indicator = 1
			0x00,       // section_number
			0x00,       // last_section_number
			0xF0, 0x23, // transmission_info_loop_length
			0xC2, 0x07, // network_identification_descriptor
			0x4A, 0x50, 0x4E, // country_code = "JPN"
			0x54, 0x42,       // media_type = "TB"
			0x7F, 0xF0,       // network_id
		};
		LibISDB::DateTime Date;
		Date.Year = 2024;
		Date.Month = 1;
		Date.Day = 2;
		const uint16_t MJD = LibISDB::DateTimeToMJDTime(Date);
		const uint8_t TransmissionDescriptors[] = {
			0xC3, 0x12, // partial_transport_stream_time_descriptor
			0x00,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF,
			0x00, 0x30, 0x00,
			0xF9,
			static_cast<uint8_t>(MJD >> 8), static_cast<uint8_t>(MJD), 0x13, 0x00, 0x00,
			0xCD, 0x04, 0x05, 0x08, 0x54, 0x53, // TS information: key 5, name "TS"
		};
		Section.insert(Section.end(), std::begin(TransmissionDescriptors), std::end(TransmissionDescriptors));

		if (InvalidTransmissionInfoLength)
			Section[9] = 0x7F;

		for (uint8_t Status = 0; Status < 8; Status++) {
			Append16(&Section, static_cast<uint16_t>(0x0400 + Status));
			if (Status == 0) {
				const uint8_t ServiceDescriptors[] = {
					0xC3, 0x0D,
					0x01,
					static_cast<uint8_t>(MJD >> 8), static_cast<uint8_t>(MJD), 0x12, 0x00, 0x00,
					0x01, 0x00, 0x00,
					0x00, 0x30, 0x00,
					0xFC,
					0x48, 0x07, 0x01, 0x02, 0x54, 0x53, 0x02, 0x54, 0x53,
					0x4D, 0x09, 0x6A, 0x70, 0x6E, 0x02, 0x54, 0x53, 0x02, 0x54, 0x53,
				};
				const size_t Length = std::size(ServiceDescriptors);
				Section.push_back(static_cast<uint8_t>(0x80 | (Status << 4) | (Length >> 8)));
				Section.push_back(static_cast<uint8_t>(Length));
				Section.insert(Section.end(), std::begin(ServiceDescriptors), std::end(ServiceDescriptors));
			} else {
				Section.push_back(static_cast<uint8_t>(0x80 | (Status << 4)));
				Section.push_back(0x00); // service_loop_length
			}
		}

		AppendCRC32MPEG2(&Section);
		return Section;
	}

	std::vector<uint8_t> MakePATSection()
	{
		std::vector<uint8_t> Section = {
			0x00,
			0xB0, 0x00,
			0x12, 0x34,
			0xC1,
			0x00,
			0x00,
			0x04, 0x00, 0xE1, 0x00,
		};
		AppendCRC32MPEG2(&Section);
		return Section;
	}

	LibISDB::TSPacket MakeSectionPacket(uint16_t PID, const std::vector<uint8_t> &Section)
	{
		std::array<uint8_t, LibISDB::TS_PACKET_SIZE> Data;
		Data.fill(0xFF);

		REQUIRE(Section.size() <= Data.size() - 5);
		Data[0] = 0x47;
		Data[1] = static_cast<uint8_t>(0x40 | (PID >> 8));
		Data[2] = static_cast<uint8_t>(PID);
		Data[3] = 0x10;
		Data[4] = 0x00; // pointer_field
		std::copy(Section.begin(), Section.end(), Data.begin() + 5);

		LibISDB::TSPacket Packet;
		Packet.SetData(Data.data(), Data.size());
		REQUIRE(Packet.ParsePacket() == LibISDB::TSPacket::ParseResult::OK);
		return Packet;
	}

	void StoreSITSection(LibISDB::SITTable *pTable, const std::vector<uint8_t> &Section)
	{
		LibISDB::TSPacket Packet = MakeSectionPacket(LibISDB::PID_SIT, Section);
		REQUIRE(pTable->StorePacket(&Packet));
	}

	void StoreSITSectionFragmented(
		LibISDB::SITTable *pTable, const std::vector<uint8_t> &Section, size_t FirstSectionBytes)
	{
		REQUIRE(FirstSectionBytes > 0);
		REQUIRE(FirstSectionBytes < Section.size());
		REQUIRE(FirstSectionBytes <= 181);

		std::array<uint8_t, LibISDB::TS_PACKET_SIZE> Data;
		Data.fill(0xFF);
		Data[0] = 0x47;
		Data[1] = static_cast<uint8_t>(0x40 | (LibISDB::PID_SIT >> 8));
		Data[2] = static_cast<uint8_t>(LibISDB::PID_SIT);
		Data[3] = 0x30;
		const size_t AdaptationFieldLength = 182 - FirstSectionBytes;
		Data[4] = static_cast<uint8_t>(AdaptationFieldLength);
		if (AdaptationFieldLength > 0)
			Data[5] = 0x00;
		const size_t PayloadPos = 5 + AdaptationFieldLength;
		Data[PayloadPos] = 0x00;
		std::copy_n(Section.begin(), FirstSectionBytes, Data.begin() + PayloadPos + 1);

		LibISDB::TSPacket Packet;
		Packet.SetData(Data.data(), Data.size());
		REQUIRE(Packet.ParsePacket() == LibISDB::TSPacket::ParseResult::OK);
		REQUIRE(pTable->StorePacket(&Packet));

		size_t SectionPos = FirstSectionBytes;
		uint8_t ContinuityCounter = 1;
		while (SectionPos < Section.size()) {
			Data.fill(0xFF);
			Data[0] = 0x47;
			Data[1] = static_cast<uint8_t>(LibISDB::PID_SIT >> 8);
			Data[2] = static_cast<uint8_t>(LibISDB::PID_SIT);
			Data[3] = static_cast<uint8_t>(0x10 | ContinuityCounter);
			const size_t CopySize = std::min(Section.size() - SectionPos, Data.size() - 4);
			std::copy_n(Section.begin() + SectionPos, CopySize, Data.begin() + 4);

			Packet.SetData(Data.data(), Data.size());
			REQUIRE(Packet.ParsePacket() == LibISDB::TSPacket::ParseResult::OK);
			REQUIRE(pTable->StorePacket(&Packet));
			SectionPos += CopySize;
			ContinuityCounter = (ContinuityCounter + 1) & 0x0F;
		}
	}

	void StoreSection(LibISDB::AnalyzerFilter *pAnalyzer, uint16_t PID, const std::vector<uint8_t> &Section)
	{
		LibISDB::TSPacket Packet = MakeSectionPacket(PID, Section);
		LibISDB::SingleDataStream<LibISDB::TSPacket> Stream(&Packet);
		REQUIRE(pAnalyzer->ReceiveData(&Stream));
	}
}


TEST_CASE("SIT standard and extended section syntax", "[ts][sit]")
{
	for (const SITSyntax Syntax : {SITSyntax::Standard, SITSyntax::Extended}) {
		LibISDB::SITTable Table;
		StoreSITSection(&Table, MakeSITSection(false, Syntax));

		CHECK(Table.GetVersionNumber() == 3);
		CHECK(Table.GetCurrentNextIndicator());
		CHECK(Table.GetSectionNumber() == 0);
		CHECK(Table.GetLastSectionNumber() == 0);
		CHECK(Table.GetNetworkID() == 0x7FF0);
		const LibISDB::DescriptorBlock *pTransmissionDescriptors = Table.GetTransmissionDescriptorBlock();
		REQUIRE(pTransmissionDescriptors != nullptr);
		REQUIRE(pTransmissionDescriptors->GetDescriptor<LibISDB::NetworkIdentificationDescriptor>() != nullptr);
		const LibISDB::TSInformationDescriptor *pTSInfo =
			pTransmissionDescriptors->GetDescriptor<LibISDB::TSInformationDescriptor>();
		REQUIRE(pTSInfo != nullptr);
		CHECK(pTSInfo->GetRemoteControlKeyID() == 5);
		LibISDB::ARIBString TSName;
		REQUIRE(pTSInfo->GetTSName(&TSName));
		CHECK(TSName.size() == 2);
		LibISDB::DateTime JSTTime;
		REQUIRE(Table.GetJSTTime(&JSTTime));
		CHECK(JSTTime.Hour == 13);
		CHECK(JSTTime.Minute == 30);
		REQUIRE(Table.GetServiceCount() == 8);

		for (int i = 0; i < 8; i++) {
			const LibISDB::SITTable::ServiceInfo *pService = Table.GetServiceInfo(i);
			REQUIRE(pService != nullptr);
			CHECK(pService->ServiceID == 0x0400 + i);
			CHECK(pService->RunningStatus == i);
		}

		const LibISDB::SITTable::ServiceInfo *pService = Table.GetServiceInfo(0);
		REQUIRE(pService != nullptr);
		REQUIRE(pService->EventTimeValid);
		CHECK(pService->EventStartTime.Hour == 11);
		CHECK(pService->EventStartTime.Minute == 30);
		CHECK(pService->EventDuration == 3600);
		REQUIRE(pService->Descriptors.GetDescriptor<LibISDB::ServiceDescriptor>() != nullptr);
		REQUIRE(pService->Descriptors.GetDescriptor<LibISDB::ShortEventDescriptor>() != nullptr);
	}
}


TEST_CASE("SIT auto syntax header can span packets", "[ts][sit]")
{
	for (const size_t FirstSectionBytes : {size_t{1}, size_t{2}, size_t{3}, size_t{7}}) {
		LibISDB::SITTable Table;
		StoreSITSectionFragmented(
			&Table, MakeSITSection(false, SITSyntax::Extended), FirstSectionBytes);

		CHECK(Table.GetVersionNumber() == 3);
		CHECK(Table.GetNetworkID() == 0x7FF0);
		CHECK(Table.GetServiceCount() == 8);
	}
}


TEST_CASE("SIT invalid loop length does not replace current table", "[ts][sit]")
{
	LibISDB::SITTable Table;
	StoreSITSection(&Table, MakeSITSection());
	REQUIRE(Table.GetServiceCount() == 8);

	for (const SITSyntax Syntax : {SITSyntax::Standard, SITSyntax::Extended}) {
		StoreSITSection(&Table, MakeSITSection(true, Syntax));

		CHECK(Table.GetNetworkID() == 0x7FF0);
		REQUIRE(Table.GetServiceCount() == 8);
		CHECK(Table.GetServiceInfo(4)->RunningStatus == 4);
	}
}


TEST_CASE("Analyzer uses SIT fallbacks", "[filter][sit]")
{
	LibISDB::AnalyzerFilter Analyzer;
	StoreSection(&Analyzer, LibISDB::PID_PAT, MakePATSection());
	StoreSection(&Analyzer, LibISDB::PID_SIT, MakeSITSection(false, SITSyntax::Extended));

	const int ServiceIndex = Analyzer.GetServiceIndexByID(0x0400);
	REQUIRE(ServiceIndex >= 0);
	CHECK(Analyzer.GetNetworkID() == 0x7FF0);
	CHECK(Analyzer.GetRemoteControlKeyID() == 5);

	LibISDB::String Text;
	REQUIRE(Analyzer.GetTSName(&Text));
	CHECK_FALSE(Text.empty());
	REQUIRE(Analyzer.GetServiceName(ServiceIndex, &Text));
	CHECK_FALSE(Text.empty());
	REQUIRE(Analyzer.GetEventName(ServiceIndex, &Text));
	CHECK_FALSE(Text.empty());

	LibISDB::DateTime JSTTime;
	REQUIRE(Analyzer.GetTOTTime(&JSTTime));
	CHECK(JSTTime.Hour == 13);
	CHECK(JSTTime.Minute == 30);

	LibISDB::EventInfo Event;
	REQUIRE(Analyzer.GetEventInfo(ServiceIndex, &Event));
	CHECK(Event.StartTime.Hour == 11);
	CHECK(Event.StartTime.Minute == 30);
	CHECK(Event.Duration == 3600);
}


TEST_CASE("Partial transport stream descriptors", "[ts][descriptor][sit]")
{
	SECTION("partial_transport_stream_descriptor") {
		const uint8_t Data[] = {
			0x63, 0x08,
			0xC1, 0x23, 0x45,
			0xC2, 0x34, 0x56,
			0xD3, 0x45,
		};
		LibISDB::PartialTransportStreamDescriptor Descriptor;

		REQUIRE(Descriptor.Parse(Data, sizeof(Data)));
		CHECK(Descriptor.GetPeakRate() == 0x012345);
		CHECK(Descriptor.GetMinimumOverallSmoothingRate() == 0x023456);
		CHECK(Descriptor.GetMaximumOverallSmoothingBuffer() == 0x1345);
	}

	SECTION("network_identification_descriptor") {
		const uint8_t Data[] = {
			0xC2, 0x09,
			0x4A, 0x50, 0x4E,
			0x54, 0x42,
			0x7F, 0xF0,
			0x12, 0x34,
		};
		LibISDB::DescriptorBlock Block;

		REQUIRE(Block.ParseBlock(Data, sizeof(Data)) == 1);
		const LibISDB::NetworkIdentificationDescriptor *pDescriptor =
			Block.GetDescriptor<LibISDB::NetworkIdentificationDescriptor>();
		REQUIRE(pDescriptor != nullptr);
		CHECK(pDescriptor->GetCountryCode() == 0x4A504E);
		CHECK(pDescriptor->GetMediaType() == 0x5442);
		CHECK(pDescriptor->GetNetworkID() == 0x7FF0);
		REQUIRE(pDescriptor->GetPrivateData()->GetSize() == 2);
		CHECK(pDescriptor->GetPrivateData()->GetAt(0) == 0x12);
		CHECK(pDescriptor->GetPrivateData()->GetAt(1) == 0x34);
	}

	SECTION("partial_transport_stream_time_descriptor with positive offset") {
		LibISDB::DateTime Date;
		Date.Year = 2024;
		Date.Month = 1;
		Date.Day = 2;
		const uint16_t MJD = LibISDB::DateTimeToMJDTime(Date);
		const uint8_t Data[] = {
			0xC3, 0x12,
			0x07,
			static_cast<uint8_t>(MJD >> 8), static_cast<uint8_t>(MJD), 0x12, 0x34, 0x56,
			0x01, 0x02, 0x03,
			0x00, 0x30, 0x00,
			0xFB,
			static_cast<uint8_t>(MJD >> 8), static_cast<uint8_t>(MJD), 0x13, 0x00, 0x00,
		};
		LibISDB::PartialTransportStreamTimeDescriptor Descriptor;

		REQUIRE(Descriptor.Parse(Data, sizeof(Data)));
		CHECK(Descriptor.GetEventVersionNumber() == 7);
		CHECK(Descriptor.IsEventDurationValid());
		CHECK(Descriptor.GetEventDuration() == 3723);
		CHECK(Descriptor.GetOffset() == 1800);
		CHECK_FALSE(Descriptor.GetOffsetFlag());
		CHECK(Descriptor.GetOtherDescriptorStatus());

		LibISDB::DateTime EventTime;
		REQUIRE(Descriptor.GetEventStartTime(&EventTime));
		CHECK(EventTime.Year == 2024);
		CHECK(EventTime.Month == 1);
		CHECK(EventTime.Day == 2);
		CHECK(EventTime.Hour == 13);
		CHECK(EventTime.Minute == 4);
		CHECK(EventTime.Second == 56);

		LibISDB::DateTime JSTTime;
		REQUIRE(Descriptor.GetJSTTime(&JSTTime));
		CHECK(JSTTime.Hour == 13);
		CHECK(JSTTime.Minute == 30);
		CHECK(JSTTime.Second == 0);
	}

	SECTION("optional JST length is validated") {
		const uint8_t Data[] = {
			0xC3, 0x0D,
			0x00,
			0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
			0xFF, 0xFF, 0xFF,
			0x00, 0x00, 0x00,
			0xF9,
		};
		LibISDB::PartialTransportStreamTimeDescriptor Descriptor;

		CHECK_FALSE(Descriptor.Parse(Data, sizeof(Data)));
	}
}




#ifdef LIBISDB_TEST_WMAIN

static char * ConvertArg(const wchar_t *arg)
{
	size_t max_length = std::wcslen(arg) * MB_CUR_MAX + 1;
	char *mbs = new char[max_length];

#ifdef _MSC_VER
	size_t dest_length;
	::wcstombs_s(&dest_length, mbs, max_length, arg, max_length);
#else
	std::wcstombs(mbs, arg, max_length);
#endif

	return mbs;
}

static char ** ConvertArgs(int argc, wchar_t **argv)
{
	char **args = new char *[argc + 1];

	for (int i = 0; i < argc; i++) {
		args[i] = ConvertArg(argv[i]);
	}

	args[argc] = nullptr;

	return args;
}

static void FreeArgs(int argc, char **argv)
{
	for (int i = 0; i < argc; i++)
		delete [] argv[i];
	delete [] argv;
}

int wmain(int argc, wchar_t **argv)
{
	std::setlocale(LC_ALL, "");
	char **args = ConvertArgs(argc, argv);
	int result = Catch::Session().run(argc, args);
	FreeArgs(argc, args);

	return result < 0xff ? result : 0xff;
}

#endif
