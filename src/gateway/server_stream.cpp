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

#include "gateway/server_stream.h"

namespace ae {
ServerStream::ServerStream(ActionContext action_context,
                           Server::ptr const& server)
    : action_context_{action_context},
      channel_manager_{action_context_, server},
      channel_select_stream_{action_context_, channel_manager_},
      buffer_stream_{action_context_} {
  Tie(buffer_stream_, channel_select_stream_);
}

ActionPtr<StreamWriteAction> ServerStream::Write(DataBuffer&& data) {
  return buffer_stream_.Write(std::move(data));
}

StreamInfo ServerStream::stream_info() const {
  return buffer_stream_.stream_info();
}

ServerStream::StreamUpdateEvent::Subscriber
ServerStream::stream_update_event() {
  return buffer_stream_.stream_update_event();
}

ServerStream::OutDataEvent::Subscriber ServerStream::out_data_event() {
  return buffer_stream_.out_data_event();
}

void ServerStream::Restream() { buffer_stream_.Restream(); }

}  // namespace ae
