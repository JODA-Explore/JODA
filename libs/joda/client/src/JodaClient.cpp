//
// Created by Nico Schäfer on 19/12/17.
//

#include <algorithm>
#include <locale>
#include <iostream>
#include <boost/asio/connect.hpp>
#include <glog/logging.h>
#include <joda/JodaClient.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <curses.h>
#include "CursesFWD.h"
#include "QueryRequest.h"
#include "ResultRequest.h"

void joda::network::client::JodaClient::cli() {
  bool exec = true;
  while(exec){
    auto cmd = parseCommand();
    if(cmd == "quit") exec = false;
    else{
      try {
        auto ret = QueryRequest::query(client, cmd, prefix);
        if (ret.first != 0 && ret.second != 0) {
          browseReturn(ret.first, ret.second);
        }
      } catch (JodaClientException &e) {
        handleError(e);
      } catch (JodaServerException &e) {
        handleError(e);
      }
    }
  }
}

std::string &joda::network::client::JodaClient::ltrim(std::string &str) {
  auto it2 =
      std::find_if(str.begin(), str.end(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
  str.erase(str.begin(), it2);
  return str;
}

std::string &joda::network::client::JodaClient::rtrim(std::string &str) {
  auto it1 =
      std::find_if(str.rbegin(), str.rend(), [](char ch) { return !std::isspace<char>(ch, std::locale::classic()); });
  str.erase(it1.base(), str.end());
  return str;
}

std::string joda::network::client::JodaClient::parseCommand() {
  std::string tmp;
  char delim(';');
  std::getline(std::cin, tmp, delim);
  return ltrim(rtrim(tmp));
}


void joda::network::client::JodaClient::browseReturn(unsigned long id, unsigned long max) {
  const unsigned long chunkSize = 1000;
  //init ncurses
  initscr();
  //Do not display typing
  noecho();
  //Enable special chars
  keypad(stdscr, TRUE);

  int h,w,jh = 0,jw=0;
  unsigned long i = 0,currMin = 0, currMax = chunkSize;
  bool exec = true;
  auto vec = ResultRequest::result(client, prefix, id, currMin, chunkSize, true);
  while(exec){
    getmaxyx(stdscr, h, w);
    DLOG(INFO) << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size();
    DLOG(INFO) << "h: " << h << " w: " << w ;

    if (vec.empty()) i = 0;
    else i = std::min(i, vec.size() - 1);
    //Get More


    //Write;
    WINDOW * jsonwin = nullptr;
    WINDOW * jsonpartwin = newwin(h-1,w,0,0);
    WINDOW * status = newwin(1,w-1,h-1,0);


    werase(status);
    werase(jsonpartwin);
    wrefresh(jsonpartwin);
    int strh = 0,strw = 0;
    if(i < vec.size()){
      getStringWindowSize(vec[i],strh,strw);
      DLOG(INFO) << "String: " << vec[i] ;
      DLOG(INFO) << "Spatial size of json string (h,w): (" << strh <<","<<strw<<")";
      jsonwin = newpad(strh+1,strw+5);
      waddstr(jsonwin,vec[i].c_str());
      prefresh(jsonwin,jh,jw,0,0,h-2,w-1);
    }

    std::string statusstr = "ID: " + std::to_string(id) + " showing " + std::to_string(currMin+i+1) + " of " + std::to_string(max);
    wprintw(status,statusstr.c_str());
    //Refresh

    wrefresh(status);

    //Get Input
    int ch = getch();

    switch (ch){
      case KEY_LEFT:
        DLOG(INFO) << "KEY_LEFT " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size();
        if(i == 0 ){
          if (currMin != 0) {
            currMin-=chunkSize;
            currMax-=chunkSize;
            currMin = currMin;
            currMax = std::max(chunkSize,currMax);
            vec = ResultRequest::result(client, prefix, id, currMin, chunkSize, true);
            i = vec.size()-1;
            jh = jw = 0;
          }

        }else{
          i--;
          jh = jw = 0;
        }
        break;
      case KEY_RIGHT:
        DLOG(INFO) << "KEY_RIGHT " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size();
        if(currMin+i < max){//Only go to max
          jh = jw = 0;
          if(currMin+i == currMax-1){
            currMin+=chunkSize;
            currMax+=chunkSize;
            i = 0;
            vec = ResultRequest::result(client, prefix, id, currMin, chunkSize, true);
          }else{
            i++;
          }
        }
        break;
      case 'w':
        jh--;
        jh = std::max(0,jh);
        DLOG(INFO) << "W " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size() << " jh: "<<jh<<" jw: "<<jw;
        break;
      case 's':
        jh++;
        jh = std::min(std::max(strh-(h-2),0),jh);
        DLOG(INFO) << "S " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size() << " jh: "<<jh<<" jw: "<<jw;
        break;
      case 'a':
        jw--;
        jw = std::max(0,jw);
        DLOG(INFO) << "A " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size() << " jh: "<<jh<<" jw: "<<jw;
        break;
      case 'd':
        jw++;
        jw = std::min(std::max(strw-(w-1),0),jw);
        DLOG(INFO) << "D " << "i: " << i << " currMin: " << currMin << " currMax: " << currMax << " chunkSize: " << chunkSize << " |vec|: " << vec.size() << " jh: "<<jh<<" jw: "<<jw;
        break;
      case 'q':
        exec = false;
        break;
      default:
        break;
    }
    if(jsonwin != nullptr)
      delwin(jsonwin);
    delwin(status);
    delwin(jsonpartwin);
  }

  //end ncurses mode
  endwin();
}


void joda::network::client::JodaClient::getStringWindowSize(const std::string &str, int &y, int &x) {
  y = 1;
  x = 1;
  int currx = 0;
  for (auto &&ch : str) {
    if(ch == '\n'){
      y++;
      currx = 0;
    }else{
      currx++;
      if(currx > x) x = currx;
    }
  }
}

void joda::network::client::JodaClient::nonInteractiveQuery(const std::string &query, unsigned long offset, unsigned long count) {
  try {
    auto ret = QueryRequest::query(client, query, prefix);
    if (ret.first == 0 && ret.second == 0) {
      return;
    }

    auto id = ret.first;
    auto size = ret.second;

    if (offset > size) return;
    count = std::min(size - offset, count);
    if (count == 0) return;

    auto curroffset = offset;
    auto currcount = std::min(count, (unsigned long) JODA_CLIENT_NON_INTERACTIVE_BATCH_SIZE);

    while (count > 0) {
      LOG(INFO) << "Getting result " << id << ". Retrieving" << currcount << " entries, starting at " << curroffset
                << ".";
      auto jsons = ResultRequest::result(client, prefix, id, curroffset, currcount, true);
      LOG(INFO) << "Retrieved " << jsons.size() << " results.";
      count -= currcount;
      curroffset += currcount;
      std::min((size - offset) - curroffset, (unsigned long) JODA_CLIENT_NON_INTERACTIVE_BATCH_SIZE);

      //Output jsons
      for (auto &&item : jsons) {
        std::cout << item << std::endl;
      }
    }

  } catch (JodaClientException &e) {
    handleError(e);
  } catch (JodaServerException &e) {
    handleError(e);
  }

}

joda::network::client::JodaClient::JodaClient(const std::string &addr, int port) : client(addr.c_str(), port, JODA_CLIENT_TIMEOUT) {
  client.set_read_timeout(JODA_CLIENT_TIMEOUT, JODA_CLIENT_TIMEOUT * 1000);
}

void joda::network::client::JodaClient::handleError(JodaClientException &e) {
  std::cerr << e.what() << std::endl;
  LOG(ERROR) << e.what();
}

void joda::network::client::JodaClient::handleError(JodaServerException &e) {
  std::cerr << e.what() << std::endl;
  LOG(ERROR) << e.what();
}

void joda::network::client::JodaClient::checkError(const rapidjson::Document &doc) {
  if (!doc.HasMember("error")) return;
  auto &error = doc["error"];
  if (!error.IsString()) throw (JodaServerAPIErrorException("'error' is not a string, invalid API response"));

  throw (JodaServerAPIErrorException(error.GetString()));
}

rapidjson::Document joda::network::client::JodaClient::parseResponse(httplib::Response &res) {
  rapidjson::Document doc;
  doc.Parse(res.body.c_str());
  if (doc.HasParseError()) {
    DLOG(ERROR) << "Error parsing response: " << res.body;
    throw (JodaServerInvalidJSONException(rapidjson::GetParseError_En(doc.GetParseError())));
  }
  return doc;
}


