/*
 * Copyright 2025 Aethernet Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GATEWAY_SERVER_STREAM_MANAGER_H_
#define GATEWAY_SERVER_STREAM_MANAGER_H_

#include <map>
#include <memory>

#include "aether/all.h"

namespace ae {
class Gateway;

namespace server_stream_manager_internal {
class RequestServerStreamGetAction;
}

/**
 * \brief Action to access generated Stream.
 */
class StreamGetAction : public Action<StreamGetAction> {
 public:
  using Action::Action;

  virtual UpdateStatus Update() = 0;
  virtual std::shared_ptr<ByteIStream> const& stream() const = 0;
};

class ServerStreamManager {
  friend class server_stream_manager_internal::RequestServerStreamGetAction;

 public:
  explicit ServerStreamManager(Gateway& gateway);

  /**
   * \brief Get stream based on existing or newly resolved server by its id.
   * \param server_id Server id.
   * \param cache Whether to use cache.
   * \return ActionPtr<StreamGetAction> Stream get action.
   */
  ActionPtr<StreamGetAction> GetStream(ServerId server_id, bool cache = true);
  /**
   * \brief Get stream based on existing or newly created server by its
   * descriptor.
   * \param server_descriptor Server id.
   * \param cache Whether to use cache.
   * \return ActionPtr<StreamGetAction> Stream get action.
   */
  ActionPtr<StreamGetAction> GetStream(
      ServerDescriptor const& server_descriptor, bool cache = true);

 private:
  Server::ptr BuildServer(ServerDescriptor const& descriptor);
  std::shared_ptr<ByteIStream> MakeStream(Server::ptr server);
  void CacheStream(ServerId server_id,
                   std::shared_ptr<ByteIStream> const& stream);

  std::map<ServerId, std::weak_ptr<ByteIStream>>& OpenCache();

  Gateway* gateway_;
  std::map<ServerId, std::weak_ptr<ByteIStream>> stream_cache_;
};
}  // namespace ae

#endif  // GATEWAY_SERVER_STREAM_MANAGER_H_
