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

#ifndef GATEWAY_GW_STREAM_H_
#define GATEWAY_GW_STREAM_H_

#include <memory>

#include "aether/all.h"

namespace ae {
class Gateway;
class GwStream : public ByteIStream {
 public:
  // Make GwStream but it's required to resolve the server
  GwStream(Gateway& gateway, ServerId server_id);
  // Make GwStream with server descriptor provided
  GwStream(Gateway& gateway, ServerDescriptor const& descriptor);

  ActionPtr<StreamWriteAction> Write(DataBuffer&& data) override;
  StreamUpdateEvent::Subscriber stream_update_event() override;
  StreamInfo stream_info() const override;
  OutDataEvent::Subscriber out_data_event() override;
  void Restream() override;

 private:
  explicit GwStream(Gateway& gateway);

  Gateway* gateway_;
  Subscription get_sererver_stream_sub_;
  std::shared_ptr<ByteIStream> server_stream_;
  BufferStream<DataBuffer> buffer_stream_;
};
}  // namespace ae

#endif  // GATEWAY_GW_STREAM_H_
