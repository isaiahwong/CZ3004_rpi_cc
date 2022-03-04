import warnings
from threading import Lock
from queue import Queue, PriorityQueue, Empty


class Pub():
    def __init__(self, max_queue_in_a_channel=100, max_id_4_a_channel=2**31):
        """
        parameters :
        - max_queue_in_a_channel : (be careful, modify if necessary)
            - Maximum number of message in a channel.
        - max_id_4_a_channel : (don't modify)
            - Maximum value for message 'id' field value on a
              communication channel.
              Used to prevent negative message ids
              to appear when number of messages broadcasted by
              this channel is very big.
            - Default value: 2**31
        """

        self.max_queue_in_a_channel = max_queue_in_a_channel
        self.max_id_4_a_channel = max_id_4_a_channel

        self.channels = {}
        self.count = {}

        self.channels_lock = Lock()
        self.count_lock = Lock()

    def subscribe(self, channel):
        """
        Return a synchronised FIFO queue object used by a subscriber
        to listen at messages sent by publishers on a given channel.
        """

        if not channel:
            raise ValueError('channel : None value not allowed')

        if channel not in self.channels:
            self.channels_lock.acquire()
            # Need to check again
            if channel not in self.channels:
                self.channels[channel] = []
            self.channels_lock.release()

        message_queue = None
        message_queue = Sub(self, channel)
        self.channels[channel].append(message_queue)

        return message_queue

    def unsubscribe(self, channel, message_queue):
        """
        Used by a subscriber who doesn't want to receive messages
        on a given channel and on a queue (message_queue)
        obtained previously by subscribe method.
        """
        if not channel:
            raise ValueError('channel : None value not allowed')
        if not message_queue:
            raise ValueError('message_queue : None value not allowed')
        if channel in self.channels:
            self.channels[channel].remove(message_queue)

    def publish(self, channel, message):
        """
        Called by publisher.
        Send a message to all subscribers registered on channel.
        Channel is created if it does not exists.
        If channel overflows, I.E. the message in channel is bigger 
        than max_queue_in_a_channel; a warning and ignore message is produced.
        Parameters :
            - channel : a string identifying the channel
            - message : payload that will be carried by the message.
        """

        if not channel:
            raise ValueError('channel : None value not allowed')
        # TODO: convert to type checking
       # if not message:
        #    raise ValueError('message : None value not allowed')

        if channel not in self.channels:
            self.channels_lock.acquire()
            # Need to check again
            if channel not in self.channels:
                self.channels[channel] = []
            self.channels_lock.release()

        # Update message self.counts
        self.count_lock.acquire()
        if channel not in self.count:
            self.count[channel] = 0
        else:
            self.count[channel] = ((self.count[channel] + 1) %
                                   self.max_id_4_a_channel)
        self.count_lock.release()

        # ID of current message
        _id = self.count[channel]

        # Push message to all subscribers in channel
        for channel_queue in self.channels[channel]:
            # Check if queue overflowed
            if channel_queue.qsize() >= self.max_queue_in_a_channel:
                warnings.warn((
                    f"Queue overflow for channel {channel}, "
                    f"> {self.max_queue_in_a_channel} "
                    "(self.max_queue_in_a_channel parameter)"))
            else:  # No overflow on this channel_queue
                # Build and send message for this queue
                channel_queue.put({'data': message, 'id': _id},
                                  block=False)


class Sub(Queue):
    """
    A FIFO queue for a channel.
    """

    def __init__(self, parent, channel):
        """
        Parameters :
        - parent : publisher parent
        - channel : string for the name of the channel
        """
        super().__init__()
        self.parent = parent
        self.name = channel

    def listen(self, block=True, timeout=None):
        """
        Called by a subscriber to get messages from a channel.
        Sub Queue is an iterator that is used to get messages sent by a
        publisher.
        Iterator can be casted to std list to get all messages
        I.E. msgs = list(messageQueue.listen(block=False))
        Message is a dictionary with 2 keys:
            'data' : the message's payload that was put in the queue by
                        publishers (see publish() method).
            'id' : Number of this message on the current channel
        Parameters :
        - block (default value: True) and timeout (default value: None)
            and behaviours if no message is in the queue.
        - timeout : None : no timeout or positive integer
        see get() in docs https://docs.python.org/3/library/queue.html
        """

        while True:
            try:
                data = self.get(block=block, timeout=timeout)
                assert isinstance(data, dict) and len(data) == 2,\
                    "Bad data in chanel queue !"
                yield data
            except Empty:
                return

    def unsubscribe(self):
        self.parent.unsubscribe(self.name, self)
