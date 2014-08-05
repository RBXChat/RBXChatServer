<?php
	require_once("robloxapi_globals.php");
	require_once("rbxutility.class.php");
	class User {
		protected $id = 0;
		protected $username = "";
		protected $friendIDList = array();
		protected $placeIDList = array();
		protected $ownedSetIDList = array();
		protected $subscribedSetIDList = array();
		protected $groupIDList = array();
		protected $playerPoints = 0;
		protected $clanPoints = 0;
		protected $bodyColors = array();
		protected $wornAssetIDList = array();
		protected $ownedAssetIDList = array();
		//protected $avatarAssetID = 0;			What's this I don't even
		protected $membershipType = 0;
		protected $robloxBadgeList = array();
		protected $numberOfFriends = 0;
		protected $forumPostCount = 0;
		protected $placeVisitsTotal = 0;
		protected $knockouts = 0;
		protected $highestVotingAccuracy = 0;
		protected $blurb = "";
		protected $timeLastSeenOrLocation = "";
		protected $isOnline = false;
		//protected $onlinePlaceID = 0;
		protected $favorites = array(
			"Heads" => array(),
			"Faces" => array(),
			"Gear" => array(),
			"Hats" => array(),
			"T-Shirts" => array(),
			"Shirt" => array(),
			"Pants" => array(),
			"Decals" => array(),
			"Models" => array(),
			"Places" => array()
		);
		protected $primaryGroupID = 0;

		function getID() {
			return $this->id;
		}
		function getUsername() {
			return $this->username;
		}
		function getPrimaryGroupID() {
			return $this->primaryGroupID;
		}
		function getWornAssetIDArray() {
			return $this->wornAssetIDList;
		}
		function addFriendsToList($json, $pageid) {
			for (
				$i=0;
				$i<sizeof($json);
				$i++
				)
				$this->friendIDList[($pageid-1)*50+$i] = $json[$i]->{'Id'};
		}
		function getFriendIDArray() {
			return $this->friendIDList;
		}
		function getBodyColorsArray() {
			return $this->bodyColors;
		}
		function getBodyColorsAsStringArray() {
			$temparr = array();
			for (
				$i = 0;
				$i < sizeof($this->bodyColors);
				$i++
			)
				array_push($temparr, RBXUtility::convertBodyColorToString($temparr[$i]));
			return $temparr;
		}
		function getBlurb() {
			return $this->blurb;
		}

		function __getJSON() {
			return json_encode(get_object_vars($this));
		}

		function __construct($userid, $generaloptions, $ownassetopt, $favassetopt) {
			if ($generaloptions & ROBLOXAPI_USER_USERNAME) {
				//////////////////////////////// USERNAME CODE
				try {
					$tempusernameinfo =
						file("http://api.roblox.com/Users/$userid");
					if (!$tempusernameinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get User Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempusernameinfo = json_decode($tempusernameinfo[0]);
				$this->username = $tempusernameinfo->{'Username'};
			}
			if ($generaloptions & ROBLOXAPI_USER_ID)
				$this->id = (int) $userid;


			//////////////////////////////// PRIMARY GROUP CODE
			if ($generaloptions & ROBLOXAPI_USER_PRIMARYGROUP) {
				try {
					$tempprimaryinfo =
						file("http://roblox.com/Groups/GetPrimaryGroupInfo.ashx?users=".$this->username);
					if (!$tempprimaryinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Primary Group Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempprimaryinfo = json_decode($tempprimaryinfo[0])->{$this->username};
				$this->primaryGroupID = $tempprimaryinfo->{'GroupId'};
			}


			//////////////////////////////// PLACE LIST CODE
			if ($generaloptions & ROBLOXAPI_USER_PLACES) {
				try {
					$tempplaceinfo =
						file("http://roblox.com/Contests/Handlers/Showcases.ashx?userId=$userid");
					if (!$tempplaceinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Place List Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempplaceinfo = json_decode($tempplaceinfo[0])->{'Showcase'};
				for ($i=0;
					 $i<sizeof($tempplaceinfo);
					 $i++
				)
					$this->placeIDList[] = $tempplaceinfo[$i]->{'ID'};
			}


			//////////////////////////////// BODY COLOR CODE
			if ($generaloptions & ROBLOXAPI_USER_BODYCOLORS) {
				$fp = fsockopen("www.roblox.com", 80);
				$input  = "GET http://www.roblox.com/Asset/BodyColors.ashx?userId=$userid HTTP/1.1\r\n";
				$input .= "Accept: */*\r\n";
				$input .= "Connection: close\r\n";
				$input .= "Content-Type: application/x-www-from-urlencoded\r\n";
				$input .= "Content-Length: " . strlen("userId=$userid"). "\r\n";
				$input .= "Host: www.roblox.com\r\n";
				$input .= "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36\r\n";
				$input .= "Accept-Encoding: gzip,deflate,sdch\r\n";
				$input .= "Accept-Language: en-US;q=0.8,fi;q=0.6\r\n\r\n";
				$input .= "userId=$userid\r\n";
				fwrite($fp, $input);
				$result = "";
				while (!feof($fp))
					$result .= fgetc($fp);
				fclose($fp);
				$colorurl = "";
				$result = explode("\r\n", $result);
				for (
					$i=0;
					$i<sizeof($result);
					$i++
				)
					if (strstr($result[$i], "Location: ")!==false)
						$colorurl = explode(": ", $result[$i])[1];
				try {
					$tempbodycolorinfo =
						file($colorurl);
					if (!$tempbodycolorinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Body Color Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempbodycolorinfo = new SimpleXMLElement($tempbodycolorinfo[0]);
				foreach (
					$tempbodycolorinfo->Item->Properties->int 
					as $bodycolor
				)
					$this->bodyColors[] = (int) $bodycolor;
			}


			//////////////////////////////// WORN ASSET CODE
			if ($generaloptions & ROBLOXAPI_USER_WORNASSETS) {
				try {
					$tempworninfo =
						file("http://roblox.com/Asset/AvatarAccoutrements.ashx?userId=$userid");
					if (!$tempworninfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Worn Asset Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempworninfo = explode(";", $tempworninfo[0]);
				for (
					$i=1; // The first asset is BodyColors.ashx, which we will ignore
					$i<sizeof($tempworninfo);
					$i++
				)
					$this->wornAssetIDList[] = (int) explode("?id=", $tempworninfo[$i])[1];
			}


			//////////////////////////////// NUMBER OF FRIENDS CODE
			if (
				   $generaloptions & ROBLOXAPI_USER_STATS 
				or $generaloptions & ROBLOXAPI_USER_PP_CP 
				or $generaloptions & ROBLOXAPI_USER_OWNEDASSETS 
				or $generaloptions & ROBLOXAPI_USER_GROUPS
				or $generaloptions & ROBLOXAPI_USER_BLURB
				or $generaloptions & ROBLOXAPI_USER_FAVORITES
				or $generaloptions & ROBLOXAPI_USER_MEMBERSHIPTYPE
				or $generaloptions & ROBLOXAPI_USER_ISONLINE
				or $generaloptions & ROBLOXAPI_USER_LASTSEEN
				or $generaloptions & ROBLOXAPI_USER_RBXBADGES) {
				try {
					$tempuserinfopage =
						file("http://www.roblox.com/User.aspx?id=$userid");
					if (!$tempuserinfopage) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Friend Number Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempnumfriendsstr = "";
				for (
					$i=0;
					$i<sizeof($tempuserinfopage);
					$i++
				) // Why does it have to be this difficult, file()?
					$tempnumfriendsstr .= $tempuserinfopage[$i] . PHP_EOL;
				$temppointinfo = phpQuery::newDocument($tempnumfriendsstr);

				if ($generaloptions & ROBLOXAPI_USER_STATS) {
					$tempnumfriends = phpQuery::newDocument($tempnumfriendsstr);
					$tempnumfriendsint = (int) str_replace(",", "", $tempnumfriends["#ctl00_cphRoblox_rbxUserStatisticsPane_lFriendsStatistics"]->html()); // Find the friend element on the page, then remove the ,'s in it and finally cast it to integer
					//$tempnumfriends = $tempnumfriends["body"]->find("#ctl00_cphRoblox_rbxUserStatisticsPane_lFriendsStatistics")->html();
					$tempnumposts = (int) str_replace(",", "", $tempnumfriends["#ctl00_cphRoblox_rbxUserStatisticsPane_lForumPostsStatistics"]->html());
					$tempnumplacevis = (int) str_replace(",", "", $tempnumfriends["#ctl00_cphRoblox_rbxUserStatisticsPane_lPlaceVisitsStatistics"]->html());
					$tempnumkos = (int) str_replace(",", "", $tempnumfriends["#ctl00_cphRoblox_rbxUserStatisticsPane_lKillsStatistics"]->html());
					$tempvotingacc = (int) $tempnumfriends["#ctl00_cphRoblox_rbxUserStatisticsPane_lHighestEverVotingAccuracyStatistics"]->html();

					$this->numberOfFriends = $tempnumfriendsint;
					$this->forumPostCount = $tempnumposts;
					$this->placeVisitsTotal = $tempnumplacevis;
					$this->knockouts = $tempnumkos;
					$this->highestVotingAccuracy = $tempvotingacc;
				}

				//////////////////////////////// PLAYER / CLAN POINTS CODE
				if ($generaloptions & ROBLOXAPI_USER_PP_CP) {
					$this->playerPoints = (int) str_replace(",", "", $temppointinfo["span.roblox-se-player-points"]->attr("title"));
					$this->clanPoints = (int) str_replace(",", "", $temppointinfo["span.roblox-se-clan-points"]->attr("title"));
				}


				//////////////////////////////// OWNED ASSET LIST CODE
				if ($generaloptions & ROBLOXAPI_USER_OWNEDASSETS) {
					// HEADS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_HEAD) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "head");
						$this->ownedAssetIDList["Heads"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// FACES
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_FACE) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "face");
						$this->ownedAssetIDList["Faces"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// GEARS
					if ($ownassetpot & ROBLOXAPI_ASSETTYPE_GEAR) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "gear");
						$this->ownedAssetIDList["Gear"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// HATS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_HAT) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "hat");
						$this->ownedAssetIDList["Hats"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// T-SHIRTS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_T_SHIRT) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "tshirt");
						$this->ownedAssetIDList["T-Shirts"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// SHIRTS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_SHIRT) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "shirt");
						$this->ownedAssetIDList["Shirts"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// PANTS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_PANTS) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "pants");
						$this->ownedAssetIDList["Pants"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// DECALS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_DECAL) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "decal");
						$this->ownedAssetIDList["Decals"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// MODELS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_MODEL) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "model");
						$this->ownedAssetIDList["Models"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// PLUGINS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_PLUGIN) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "plugin");
						$this->ownedAssetIDList["Plugins"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// ANIMATIONS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_ANIMATION) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "anim");
						$this->ownedAssetIDList["Animations"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// PLACES
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_PLACE) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "place");
						$this->ownedAssetIDList["Places"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// GAME PASSES
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_GAMEPASS) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "pass");
						$this->ownedAssetIDList["Game Passes"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// AUDIOS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_AUDIO) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "audio");
						$this->ownedAssetIDList["Audio"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// BADGES
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_BADGE) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "badge");
						$this->ownedAssetIDList["Badges"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// LEFT ARMS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_LEFT_ARM) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "larm");
						$this->ownedAssetIDList["Left Arms"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// RIGHT ARMS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_RIGHT_ARM) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "rarm");
						$this->ownedAssetIDList["Right Arms"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// LEFT LEGS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_LEFT_LEG) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "lleg");
						$this->ownedAssetIDList["Left Legs"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// RIGHT LEGS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_RIGHT_LEG) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "rleg");
						$this->ownedAssetIDList["Right Legs"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// TORSOS
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_TORSO) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "torso");
						$this->ownedAssetIDList["Torsos"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}

					// PACKAGES
					if ($ownassetopt & ROBLOXAPI_ASSETTYPE_PACKAGE) {
						$tempuassetinfo = RBXUtility::switchAssetType($temppointinfo, $userid, "package");
						$this->ownedAssetIDList["Packages"] = RBXUtility::getAssets($tempuassetinfo, $userid);
					}
				}
				//////////////////////////////// GROUP LIST CODE
				if ($generaloptions & ROBLOXAPI_USER_GROUPS)
					$this->groupIDList = RBXUtility::getGroups($userid, $temppointinfo);

				//////////////////////////////// BLURB CODE
				if ($generaloptions & ROBLOXAPI_USER_BLURB)
					$this->blurb = preg_replace("/(<br>)+/", "\n", $temppointinfo[".UserBlurb"]->html());

				//////////////////////////////// FAVORITES CODE
				if ($generaloptions & ROBLOXAPI_USER_FAVORITES) {
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_HEAD) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "head");
						$this->favorites["Heads"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_FACE) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "face");
						$this->favorites["Faces"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_GEAR) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "gear");
						$this->favorites["Gear"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_HAT) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "hat");
						$this->favorites["Hats"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_T_SHIRT) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "tshirt");
						$this->favorites["T-Shirts"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_SHIRT) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "shirt");
						$this->favorites["Shirts"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_PANTS) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "pants");
						$this->favorites["Pants"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_DECAL) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "decal");
						$this->favorites["Decals"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_MODEL) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "model");
						$this->favorites["Models"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
					if ($favassetopt & ROBLOXAPI_ASSETTYPE_PLACE) {
						$tempuassetinfo = RBXUtility::switchFavoriteType($temppointinfo, $userid, "place");
						$this->favorites["Places"] = RBXUtility::getFavorites($userid, $tempuassetinfo);
					}
				}

				//////////////////////////////// MEMBERSHIP TYPE CODE
				if ($generaloptions & ROBLOXAPI_USER_MEMBERSHIPTYPE) {
					$src = $temppointinfo['#ctl00_cphRoblox_rbxUserPane_AvatarImage img[src*="/images"]'];
					if ($src->length()) {
						switch ($src->attr("src")) {
							case "/images/icons/overlay_obcOnly.png":
								$this->membershipType = 3;
								break;
							case "/images/icons/overlay_tbcOnly.png":
								$this->membershipType = 2;
								break;
							case "/images/icons/overlay_bcOnly.png":
								$this->membershipType = 1;
								break;
						}
					}
					else
						$this->membershipType = 0;
				}

				//////////////////////////////// IS ONLINE CODE
				if ($generaloptions & ROBLOXAPI_USER_ISONLINE) {
					$offline = $temppointinfo[".UserOfflineMessage"];
					if (!$offline->length())
						$this->isOnline = true;
				}

				//////////////////////////////// LAST SEEN CODE
				if ($generaloptions & ROBLOXAPI_USER_LASTSEEN) {
					$offline = $temppointinfo[".UserOfflineMessage"];
					if (!$offline->length()) {
						$data = [
							'ctl00$cphRoblox$SearchTextBox' => $this->username,
							'ctl00$cphRoblox$SearchButton' => "Search Users"
						];
						$fp = fsockopen("209.15.15.253", 80);
						$reqstr  = "POST http://www.roblox.com/Browse.aspx HTTP/1.1\r\n";
						$reqstr .= "Host: www.roblox.com\r\n";
						$reqstr .= "Connection: close\r\n";
						$reqstr .= "Content-Length: ";
						$reqstr .= 827 + strlen($this->username) . "\r\n";
						$reqstr .= "Cache-Control: max-age=0\r\n";
						$reqstr .= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n";
						$reqstr .= "Origin: http://www.roblox.com\r\n";
						$reqstr .= "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.153 Safari/537.36\r\n";
						$reqstr .= "Content-Type: application/x-www-form-urlencoded\r\n";
						$reqstr .= "Referer: http://www.roblox.com/Browse.aspx\r\n";
						$reqstr .= "Accept-Encoding: gzip,deflate,sdch\r\n";
						$reqstr .= "Accept-Language: en-US,en;q=0.8,fi;q=0.6\r\n";
						$reqstr .= "Cookie: RBXMarketing=FirstHomePageVisit=1; RBXSource=rbx_acquisition_time=6/14/2014 2:41:05 AM&rbx_acquisition_referrer=&rbx_medium=Direct&rbx_source=&rbx_campaign=&rbx_adgroup=&rbx_keyword=&rbx_matchtype=&rbx_send_info=1; rbx-ip=; __RequestVerificationToken=KP4Ek7xd-QgU9Qux3iyMf4NktQbLV6MhtqbTRuK6VenPL2NI3IBoI0QOoTHnb4f1aumv36vw9lD3dW10fnDc_kfuRolt_iXEu5piBflUoiFnJx-dsDYkZtDipTSl5B51zMzthw2; .ROBLOSECURITY=7A896752FDCE94448E16EC6755D3DA4875EBB274B576565219DAEF4BCAE1230395638AFFFEA565040A3B3AD373DC4C9109DC276BE3C385F176B072779E9D2C6282EAC32C227DFA0945BC8E9C0C2B8440A8711287B532587C7F1C310F219E78BF795B8F3249BBA792712D5699D73FEF1AC9B54C3308AD0BFEC202B3D10965AE2BB4C3796217FCBE8B5866F9073D7CF60147DAF397DFC692ED81EA7499A0C860FF73C216D741B43AE62D040E9710B37D7C88838A81CA2DBA79767A489ECDE344F5839C8C94AE5162D74C778131015037211DA0333738C8DCACC0D93ADE287EF3B89D95FFA8427FEDAC40451D55570411216E9E64F6FDDDE0A578D72EBBA6B7DAEE857C8B3A7D360A0DBD4AEF3AD928EA09AA5BBCB474F4EF0B4F9E7C79FE5DB01F582316F9C181BB4563C2E1BD074FCFF673C961C47698B5D147F98C440F0BF40F0E0EF06B39BEAEDA158FA8A309C8B08ED194713A9339A47436A4BBD8F2EC69BE26956200D55895638FB834A65EBB7E576B774099BE1BE6F7FBD18D2687D19A79C5D77CA46C9126A45D28880EEC27771712498F9E9DCA4AB1CA167D817EDA7DFD920D88420831EF5E2CB1EBFA32B1E6F9EC593D4D34BCACF4DFC6A811939D59F996D6DBE041776D9796D3FBA00530C62CE54FB5B0EE63192018EF5EDD6FA6E3F11089EDE4A81808B1CB2BCAE17E12A792A2512FD59E1E4DCC36E69E14BE7A4250434EBD587D9F674F3E6AFD6077FC6F26E322086C8E8B3ED1FB48AB29D1765DE3FEE5B675C9F5BC54AA994FC0F7599668E6F094FA13F54118039C28E6667CE4A9762B0EB7CDE50168F3CA13A647521A2B443A5993E854814C0EB933E92CDA7BED655590E8D7E8536B06D51A5735D556A88288FDC7; RBXEventTracker=CreateDate=7/3/2014 4:18:11 AM&rbxid=60386214&browserid=1625503129; rbx-ForumSync=UserID=61441223; RobloLang=language=en&country=US; RBX-MCf=360944185; __unam=3eb176a-1467bf167f1-76c0101b-29; GuestData=UserID=-2022207222&Gender=1; __utma=200924205.2064336977.1402133301.1404480897.1404491379.115; __utmb=200924205.65.0.1404491484362; __utmc=200924205; __utmz=200924205.1402476781.23.4.utmcsr=google|utmccn=(organic)|utmcmd=organic|utmctr=(not%20provided); ChatWindows=%5B%5D; CurrentTabID=bestFriendsTab_dock_thumbnails; IsMinimized=true; PartyWindowVisible=false; OnlineStatuses=%5B%7B%22userid%22%3A%222690841%22%2C%22online%22%3Afalse%7D%2C%7B%22userid%22%3A%2219233142%22%2C%22online%22%3Afalse%7D%2C%7B%22userid%22%3A%2219656891%22%2C%22online%22%3Afalse%7D%2C%7B%22userid%22%3A%2228893687%22%2C%22online%22%3Afalse%7D%5D\r\n\r\n";
						$reqstrcont = "__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=rrDsRZ0CgBRdW833kEI2sc8BQnxeDhUQX9QEgP9dSvyYMRWF0%2FO7IwEIUfB0ptg2%2B0DTPhGLqBNumyLe5OQu0ZfQitdbgZ9ChPwFmSEhWxmSXYXtbD3AI8njr0yHlq6EnvprfQbBSNFTI8psjwkPSRMuB%2FdTEUIpg2VioHcUHf7eWg9asgsD8TTbJStrbaXz%2BYRwta1O3lBbKy43pZG3MYiKfFNcSL%2BrF%2BUr9rS4uJzM7WAUDIlxBmEavWAlGPinyK97JWkL8C3xCbkrsZJVWr%2FjpinWRKcFRRROi9jhtiEN6rgccY6CC%2Bim%2FaJWNMlsQKeWXXF12jywWBl3px2B6nwsWSz03rY8p1RVB4gTcY3xX4bQJrhZd9Dk5i3HNDlLVj0SGsd4hkESspvPpD6JLu7F3QQ%3D&__VIEWSTATEENCRYPTED=&__EVENTVALIDATION=xPJXY5Q%2FV4qWGYgilXHZNoYd3CqvNZL6bKRhWOukWevbM8aNPQwgvVuZh%2BgOreb72XY1qQnZD9KWQ8uV21%2B4LI4JN8avtI%2F5XFz96XOZak7EuIV7%2F5Q4OopoIeKhMrrQD8zZEl0OL5BI7d1dsDPgnbUQgps%3D&ctl00%24cphRoblox%24SearchTextBox=";
						$reqstrcont .= $this->username;
						$reqstrcont .=  "&ctl00%24cphRoblox%24SearchButton=Search+Users&comments=&rdoOnline=on&rdoOnline2=on&rdoNotifications=on\r\n\r\n";
						//echo strlen($reqstrcont);
						//echo "\n" . 827 + strlen($this->username);
						$reqstr .= $reqstrcont;
						$ret = fwrite($fp, $reqstr);
						if ($ret < strlen($reqstr)) {
							echo "ERROR $ret";
							die();
						}
						$ret = "";
						$c = "";
						while (!feof($fp)) {
							$c = fgetc($fp);
							//var_dump($c);
							$ret .= $c;
							ob_flush();
						}
						$gz = explode("\r\n\r\n", $ret)[1];
						//var_dump($gz);
						$ret = gzdecode($gz);
						//var_dump($ret);
						//die();
						$pq = phpQuery::newDocument($ret);
						//$userlink = $pq["a[href$='$userid']"];
						//$userrow = $userlink->parent()->parent();
						$lastseencontainer = $pq["#ctl00_cphRoblox_gvUsersSearched_ctl02_lblUserLocationOrLastSeen"];
						//var_dump($lastseencontainer->length());
						$this->timeLastSeenOrLocation = $lastseencontainer->html();
					}
				}
				//////////////////////////////// ROBLOX BADGE CODE
				if ($generaloptions & ROBLOXAPI_USER_RBXBADGES) {
					//echo "Asdf";
					if ($temppointinfo["[href$='Badge2']"]->length())
						$this->robloxBadgeList[] = "Friendship";
					if ($temppointinfo["[href$='Badge3']"]->length())
						$this->robloxBadgeList[] = "Combat Initation";
					if ($temppointinfo["[href$='Badge4']"]->length())
						$this->robloxBadgeList[] = "Warrior";
					if ($temppointinfo["[href$='Badge5']"]->length())
						$this->robloxBadgeList[] = "Bloxxer";
					if ($temppointinfo["[href$='Badge8']"]->length())
						$this->robloxBadgeList[] = "Inviter";
					if ($temppointinfo["[href$='Badge6']"]->length())
						$this->robloxBadgeList[] = "Homestead";
					if ($temppointinfo["[href$='Badge7']"]->length())
						$this->robloxBadgeList[] = "Bricksmith";
					if ($temppointinfo["[href$='Badge12']"]->length())
						$this->robloxBadgeList[] = "Veteran";
					if ($temppointinfo["[href$='Badge1']"]->length())
						$this->robloxBadgeList[] = "Administrator";
					if ($temppointinfo["[href$='Badge16']"]->length())
						$this->robloxBadgeList[] = "Outrageous Builders Club";
					if ($temppointinfo["[href$='Badge9']"]->length())
						$this->robloxBadgeList[] = "Forum Moderator";
					if ($temppointinfo["[href$='Badge10']"]->length())
						$this->robloxBadgeList[] = "Image Moderator";
					if ($temppointinfo["[href$='Badge14']"]->length())
						$this->robloxBadgeList[] = "Ambassador";
					if ($temppointinfo["[href$='Badge15']"]->length())
						$this->robloxBadgeList[] = "Turbo Builders Club";
					if ($temppointinfo["[href$='Badge11']"]->length())
						$this->robloxBadgeList[] = "Builders Club";
					if ($temppointinfo["[href$='Badge13']"]->length())
						$this->robloxBadgeList[] = "Super Moderator"; // MSE6, for example
				}
			}


			//////////////////////////////// FRIEND ID LIST CODE
			if ($generaloptions & ROBLOXAPI_USER_FRIENDS) {
				try {
					$tempfriendlistinfo =
						file("http://api.roblox.com/users/$userid/friends?page=1");
					if (!$tempfriendlistinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Friend List Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempfriendlistinfo = json_decode($tempfriendlistinfo[0]);
				$this->addFriendsToList($tempfriendlistinfo, 1);
			}


			//////////////////////////////// OWNED SET LIST CODE
			if ($generaloptions & ROBLOXAPI_USER_OWNEDSETS) {
				try {
					$tempsetinfo =
						file("http://roblox.com/Sets/SetHandler.ashx?rqtype=getsets&maxsets=1&UserID=$userid"); // maxsets has no effect
					if (!$tempsetinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Set List Information for userid $userid! Does the user exist?</span></b>");
				}
				$tempsetinfo = json_decode($tempsetinfo[0]);
				for (
					$i=0;
					$i<sizeof($tempsetinfo);
					$i++
				)
					$this->ownedSetIDList[] = $tempsetinfo[$i]->{'ID'};
			}


			//////////////////////////////// SUBSCRIBED SET LIST CODE
			if ($generaloptions & ROBLOXAPI_USER_SUBBEDSETS) {
				try {
					$tempsubsetinfo =
						file("http://roblox.com/Sets/SetHandler.ashx?rqtype=getsubscribedsets&maxsets=1&UserID=$userid");
					if (!$tempsubsetinfo) throw new Exception();
				}
				catch (Exception $e) {
					throw new Exception("<b><span style='color: red;'>Failed to get Subscribed Set List Information for userid $userid! Does the user exist?</span></b>");
				}

				$tempsubsetinfo = json_decode($tempsubsetinfo[0]);
				for (
					$i=0;
					$i<sizeof($tempsubsetinfo);
					$i++
				)
					$this->subscribedSetIDList[] = $tempsubsetinfo[$i]->{'ID'};
			}

		}
	}
?>