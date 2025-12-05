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

#ifndef GATEWAY_SERVER_STREAM_H_
#define GATEWAY_SERVER_STREAM_H_

#include "aether/server.h"
#include "aether/stream_api/istream.h"
#include "aether/actions/action_context.h"
#include "aether/stream_api/buffer_stream.h"
#include "aether/server_connections/channel_selection_stream.h"

namespace ae::gw {
class ServerStream : public ByteIStream {
 public:
  ServerStream(ActionContext action_context, Server::ptr const& server);

  ActionPtr<StreamWriteAction> Write(DataBuffer&& data) override;
  StreamInfo stream_info() const override;
  StreamUpdateEvent::Subscriber stream_update_event() override;
  OutDataEvent::Subscriber out_data_event() override;
  void Restream() override;

 private:
  void OnStreamUpdate();

  ActionContext action_context_;
  ChannelManager channel_manager_;
  ChannelSelectStream channel_select_stream_;
  BufferStream<DataBuffer> buffer_stream_;
};
}  // namespace ae::gw

#endif  // GATEWAY_SERVER_STREAM_H_
