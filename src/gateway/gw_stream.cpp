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

#include "gateway/gw_stream.h"

#include "gateway/gateway.h"
#include "gateway/server_stream_manager.h"

namespace ae {
GwStream::GwStream(Gateway& gateway, ServerId server_id) : GwStream{gateway} {
  // TODO: add policy to select cache stream or not
  auto get_stream_action = gateway.server_stream_manager().GetStream(server_id);
  get_sererver_stream_sub_ = get_stream_action->StatusEvent().Subscribe(
      OnResult{[this](auto const& action) {
        server_stream_ = action.stream();
        Tie(buffer_stream_, *server_stream_);
      }});
}

GwStream::GwStream(Gateway& gateway, ServerEndpoints const& endpoints)
    : GwStream{gateway} {
  auto get_stream_action =
      // TODO: add policy to select cache stream or not
      gateway.server_stream_manager().GetStream(endpoints);
  get_sererver_stream_sub_ = get_stream_action->StatusEvent().Subscribe(
      OnResult{[this](auto const& action) {
        server_stream_ = action.stream();
        Tie(buffer_stream_, *server_stream_);
      }});
}

GwStream::GwStream(Gateway& gateway)
    : gateway_{&gateway}, buffer_stream_{*gateway_} {}

ActionPtr<StreamWriteAction> GwStream::Write(DataBuffer&& data) {
  return buffer_stream_.Write(std::move(data));
}

GwStream::StreamUpdateEvent::Subscriber GwStream::stream_update_event() {
  return buffer_stream_.stream_update_event();
}

StreamInfo GwStream::stream_info() const {
  return buffer_stream_.stream_info();
}

GwStream::OutDataEvent::Subscriber GwStream::out_data_event() {
  return buffer_stream_.out_data_event();
}

void GwStream::Restream() { buffer_stream_.Restream(); }

}  // namespace ae
