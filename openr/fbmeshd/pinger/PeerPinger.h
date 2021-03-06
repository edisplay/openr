/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <folly/IPAddressV6.h>
#include <folly/MacAddress.h>
#include <folly/Subprocess.h>
#include <folly/io/async/AsyncTimeout.h>
#include <folly/io/async/EventBase.h>
#include <folly/io/async/TimeoutManager.h>

#include <openr/fbmeshd/802.11s/Nl80211Handler.h>

class PeerPinger : public folly::AsyncTimeout {
  // This class should never be copied; remove default copy/move
  PeerPinger() = delete;
  PeerPinger(const PeerPinger&) = delete;
  PeerPinger(PeerPinger&&) = delete;
  PeerPinger& operator=(const PeerPinger&) = delete;
  PeerPinger& operator=(PeerPinger&&) = delete;

 public:
  PeerPinger(folly::EventBase* evb, openr::fbmeshd::Nl80211Handler& nlHandler);

  ~PeerPinger() override;

  void run();

  void stop();

  void syncPeers();

  void timeoutExpired() noexcept override;

 private:
  void pingPeer(const folly::MacAddress& peer);

  void parsePingOutput(folly::StringPiece s, folly::MacAddress peer);

  // processes collected ping data by filtering out outliers and calculating
  // the average ping delay value. This value will be stored as the link metric
  void processPingData();

  std::unordered_set<folly::MacAddress> peers_;

  // collected set of ping delay data for each peer
  std::unordered_map<folly::MacAddress, std::vector<float>> pingData_;

  // link metric calculated for each peer
  std::unordered_map<folly::MacAddress, float> linkMetric_;

  folly::EventBase* evb_;

  // netlink handler used to request peers from the kernel
  openr::fbmeshd::Nl80211Handler& nlHandler_;
};
