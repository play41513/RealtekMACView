﻿#ifndef ConstantStringH
#define ConstantStringH
//---------------------------------------------------------------------------
//
#define PASSWORD "SET"
#define MEMOWORD "MTT"
//標題
const TCHAR APP_TITLE[] = _T("Realtek MACView v1.1 (ActionStar.2019.1");
//
const AnsiString zCharacteristics[9] =
{
 "SPI_FLASH_EN" ,
 "EEPROM_EN" ,
 "NO_REMOTE_WAKEUP" ,
 "BOS_DESC_SUPERSPEED" ,
 "LPM_BESL_EN" ,
 "ECM_FLOW_CONTROL" ,
 "LAN_DIS" ,
 "LINK_OK" ,
 "GPHY_FLOW_CONTROL"
 };
 //

const AnsiString MAC_FILE_PATH  	= "RealtekMAC\\x64\\";
const AnsiString MAC_TOOL  			= "RTUNicPG64.exe";
const AnsiString MAC_LOG_PATH  		= "RealtekMAC\\x64\\pgtool.log";
const AnsiString CMD_GET_DEV  		= "/s";
const AnsiString CMD_GET_DEV_DATA  	= "/efuse /r";
const AnsiString CMD_BURN_MAC	  	= "/# 0 /efuse /w /cfgsnchg /wrsn";

const AnsiString RUN_AGAIN_TOOL		= "Please run PG tool again";
const AnsiString ANSI_NOT_FIND_DUT	= "Search NIC Adapter Failed";
const AnsiString BURN_SUCCESSFUL	= "PG EFuse is Successful";
const AnsiString EFUSE_SPCAE_FULL	= "Efuse does not have enough space to PG";
const AnsiString EFUSE_SPACE_NULL	= "EFuse Write Count = 13";
//Barcode
const AnsiString VERIFY_BARCODE		= "請掃描MAC條碼";

//ERROR_MSG
const AnsiString MSG_BARCODE_ERROR		= "(!)Barcode輸入異常";
const AnsiString MSG_USB3_ERROR			= "(!)USB3.0 Error";
const AnsiString MSG_MAC_ERROR	    	= "(!)MAC不符";
const AnsiString MSG_SN_ERROR	    	= "(!)序號不符";
const AnsiString MSG_SPI_FLASH_EN_ERROR	= "(!)SPI_FLASH_EN不符";
const AnsiString MSG_EEPROM_EN_ERROR	= "(!)EEPROM_EN不符";
const AnsiString MSG_LED_SEL_CFG_ERROR	= "(!)LED_SEL_CFG不符";
const AnsiString MSG_DOCKING_ERROR		= "(!)DOCKING不符";
const AnsiString MSG_EFUSE_ERROR		= "(!)EFuse不符 詳情於右側視窗查看";
const AnsiString MSG_EFUSE_SPACE_FULL	= "(!)DUT燒錄已滿";
const AnsiString MSG_BURN_ERROR			= "(!)燒錄失敗";
const AnsiString MSG_EFUSE_SPACE_NULL	= "(!)EFUSE內容為空";
const AnsiString MSG_LOAD_ERROR			= "(!)未載入完整Firmware樣本資訊";
const AnsiString MSG_NOT_FIND_DUT		= "(!)找不到裝置";
const AnsiString MSG_ERROR			    = "(!)異常發生";

//PROCESS_MSG
const AnsiString MSG_VERIFY_FINISH		= "驗證完成";
const AnsiString MSG_WAIT_DUT			= "等候中";
const AnsiString MSG_DUT_READY			= "開始作業";
const AnsiString MSG_DUT_READY_BURN		= "燒錄程序";
const AnsiString MSG_DUT_START_BURN		= "燒錄開始";
const AnsiString MSG_DUT_READY_VERIFY	= "驗證程序";
const AnsiString MSG_GETTING_DUT_DATA	= "取得裝置資料中...";

//
const AnsiString VERIFY_DUT_SERIAL		= "Serial Number驗證";
const AnsiString VERIFY_DUT_MAC 		= "MAC驗證";
const AnsiString VERIFY_DUT_EFUSE  		= "eFUSE驗證";


#define MB_WARNING(handle, msg) MessageBox(handle, msg, "WARNING", MB_ICONEXCLAMATION)
//---------------------------------------------------------------------------
#endif
