struct Tweet {
    int timestamp;
    int tweet_id;

    bool operator<(const Tweet& other) const {
        return timestamp < other.timestamp;
    }
};

class Twitter {
    unordered_map<int, unordered_set<int>> user_follows;
    unordered_map<int, deque<Tweet>> user_tweets;
    int timestamp = 0;

    using tweet_iter = deque<Tweet>::iterator;

public:
    Twitter() {

    }
    
    void postTweet(int userId, int tweetId) {
        user_tweets[userId].emplace_front(timestamp++, tweetId);
        if (user_tweets[userId].size() > 10)
            user_tweets[userId].pop_back();
    }
    
    vector<int> getNewsFeed(int userId) {
        user_follows[userId].insert(userId);

        auto comp = [](const Tweet& a, const Tweet& b) { return a.timestamp > b.timestamp; };

        priority_queue<Tweet, vector<Tweet>, decltype(comp)> min_heap(comp);

        for (int followeeId : user_follows[userId]) {
            auto& tweets = user_tweets[followeeId];
            for (auto& tweet : tweets) {
                min_heap.emplace(tweet);
                if (min_heap.size() > 10) {
                    min_heap.pop();
                }
            }      
        }

        vector<int> news_feed(min_heap.size());
        while (!min_heap.empty()) {
            news_feed[min_heap.size()-1] = (min_heap.top().tweet_id);
            min_heap.pop();
        }

        return news_feed;
    }
    
    void follow(int followerId, int followeeId) {
        user_follows[followerId].insert(followeeId);
    }
    
    void unfollow(int followerId, int followeeId) {
        if (followerId != followeeId)
            user_follows[followerId].erase(followeeId);
    }
};


class Twitter {
    unordered_map<int, unordered_set<int>> user_follows;
    unordered_map<int, deque<Tweet>> user_tweets;
    int timestamp = 0;

    using tweet_iter = deque<Tweet>::iterator;

public:
    Twitter() {

    }
    
    void postTweet(int userId, int tweetId) {
        user_tweets[userId].emplace_front(timestamp++, tweetId);
        if (user_tweets[userId].size() > 10)
            user_tweets[userId].pop_back();
    }
    
    vector<int> getNewsFeed(int userId) {
        user_follows[userId].insert(userId);

        priority_queue<tuple<Tweet, tweet_iter, tweet_iter>> max_heap;

        for (int followeeId : user_follows[userId]) {
            auto& tweets = user_tweets[followeeId];
            if (!tweets.empty()) {
                max_heap.emplace(tweets.front(), tweets.begin(), tweets.end());
            }
        }

        vector<int> news_feed;
        while (!max_heap.empty() && news_feed.size() < 10) {
            auto [tweet, it, end] = max_heap.top(); max_heap.pop();
            news_feed.emplace_back(tweet.tweet_id);
            it++;
            if (it != end) {
                max_heap.emplace(*it, it, end);
            }
        }

        return news_feed;
    }
    
    void follow(int followerId, int followeeId) {
        user_follows[followerId].insert(followeeId);
    }
    
    void unfollow(int followerId, int followeeId) {
        if (followerId != followeeId)
            user_follows[followerId].erase(followeeId);
    }
};

/**
 * Your Twitter object will be instantiated and called as such:
 * Twitter* obj = new Twitter();
 * obj->postTweet(userId,tweetId);
 * vector<int> param_2 = obj->getNewsFeed(userId);
 * obj->follow(followerId,followeeId);
 * obj->unfollow(followerId,followeeId);
 */