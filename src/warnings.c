#include "warnings.h"

// CPU Warning Messages
const char* cpuWarningMessages[] = {
    "Your CPU is crying. Maybe close some tabs?",
    "CPU overload! Did you launch a spaceship?",
    "The CPU is working harder than you right now.",
    "CPU at 100%! Is it running the world’s most complex calculations?",
    "Woah now, partner! Let's take that CPU temperature down a hundred degrees!",
    "This is getting out of hand! Your CPU isn't gonna make it!",
    "I know you have to flex that you have a high end computer but even THIS is a little too far!",
    "As they say in Australia, 'your CPU's almost down the pisser'",
    "Is what you're doing more important than the health of your CPU?",
    "A few tabs wouldn't hurt, would they?",
    "We're really jumping off the deep end with this temperature.",
    "In this temperature, your computer might catch heatstroke.",
    "A few more degrees until a disco inferno",
    "Either you didn't know that your CPU usage is high or you're just plain ignorant",
    "CPU usage is really high, dude! This is some f**ked up shit right here.",
    "You can light a matchstick with this heat.",
    "We're overclocking the overclocking!",
    "Yikes! Might want to turn the heat down if I were you.",
    "By the time you read this message, your computer may power off at any moment due to overexhaustion.",
    "Still working hard, are we?",
    "Even CPUs have limits, you know?",
    "Your mother will be sad by how much your CPU is suffering.",
};

const int cpuWarningMessageCount = sizeof(cpuWarningMessages) / sizeof(cpuWarningMessages[0]);

// RAM Warning Messages
const char* ramWarningMessages[] = {
    "Out of memory to create memories.",
    "Not the recommended amount of detotated wam",
    "RAM is currently in a chokehold! Please save it!",
    "Do you really need that much programs open at once?",
    "Something tells me you're not very organized.",
    "Just cause you paid for the entire RAM doesn't mean you have to use it all.",
    "Why can't we operate in a tidy, organized fashion?",
    "There's not a lot of RAM left, so why not use some of that good ol' disk space?",
    "RAM is almost fully used? Are you holding important government documents in memory?!",
    "RAM is getting a little claustrophobic in there, don't you think?",
    "Your RAM is on a vacation... And it's taking a long time to come back.",
    "If RAM had a cry button, it would have pressed it by now.",
    "Is this RAM or a hoarder's paradise? Clean it up!",
    "Who needs free space? Apparently, not your RAM!",
    "RAM is screaming for help, but nobody's listening.",
    "RAM is almost out of room! Did you invite every program you own?",
    "Who knew RAM could be this crowded? It’s like a concert in there.",
    "I hope you're not planning on running any games anytime soon!",
    "Your RAM is a real trooper, but even it has its limits.",
    "Time to set up a RAM charity. You’ve taken everything it had.",
    "No space left to breathe! Maybe try a minimalist approach?",
    "Not enough RAM left to remember your last memory.",
    "Did you sign up for RAM overload? Because it’s happening right now.",
    "RAM's not on strike, but it's close. Give it some space!",
    "Your RAM's about to file for a break. Let's help it out.",
    "If RAM could text, it would be sending 'Help!' right now.",
};

const int ramWarningMessageCount = sizeof(ramWarningMessages) / sizeof(ramWarningMessages[0]);

const char* GetRandomCpuWarningMessage() {
    srand((unsigned int)time(NULL));  // Seed the random number generator
    int randomIndex = rand() % cpuWarningMessageCount;
    return cpuWarningMessages[randomIndex];
}

const char* GetRandomRamWarningMessage() {
    srand((unsigned int)time(NULL));
    int randomIndex = rand() % ramWarningMessageCount;
    return ramWarningMessages[randomIndex];
}
