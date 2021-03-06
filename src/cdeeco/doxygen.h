/**
 * \defgroup cdeeco Core system CDECCo++ classes
 * Core classes used to implement CDEECo++ framework.
 */

/**
 * \ingroup cdeeco
 *
 * Core system CDEECo++ classes
 */
namespace CDEECO {
}


/**
 * \mainpage CDEECo++ framework
 *
 * This deals with the framework implementation including the testing project which was used during development. It
 * includes programmer documentation for the test project and the framework. Framework structure and usage description was
 * also covered in order to help other adopters of the framework.
 *
 * Used software
 * =============
 *
 * Even when the framework and the test environment were developed with standards in mind it is not always possible to
 * maintain compatibility with wide range of tools. In some places experimental features such as C++14 were used in order
 * to produce simpler and more readable code. In order to allow reproduction of the outputs of this thesis the following
 * text provides versions and settings for tools used during the development.
 *
 * Toolchain
 * ---------
 * A Toolchain is the core of every development process. It consists of basic utilities and libraries such as compiler,
 * linker and debugger. It is even more important for embedded projects as those require cross-compiler setup. The
 * toolchain used for testing and development of the framework on the STM32F4 board was created using crossdev
 * application using these commands:
 *
 * $ crossdev --target armv7m-hardfloat-eabi --ex-gcc --ex-gdb
 *
 * Unfortunately this great tool is [Gentoo linux](http://gentoo.org) specific as it uses the systems
 * package installer to build the toolchain. As a replacement toolchain provided by [ST](http://www.st.com)
 * company or custom build toolchain with the armv7m-hardfloat-eabi target triplet can be used. Hardware floating
 * point is essential for framework code to work as FreeRTOS requires it. These versions of software were
 * included in the toolchain used for development of the framework and testing application.
 *
 *
 * - binutils version: 2.24-r3 with C++ support
 * - gcc version: 4.9.0 with C++ support
 * - gdb version: 7.7.1 with XML support
 * - newlib version: 2.1.0
 *
 * Libraries
 * ---------
 * Aside from newlib library which is considered to be part of the toolchain the framework uses
 * FreeRTOS operating system and STM32F4 peripheral library. FreeRTOS is a simple operating system
 * which comes in form of a library. It contains scheduler and basic synchronization primitives. Due to nature of this
 * library the sources and other needed files found in FreeRTOS were integrated directly into the project. The
 * STM32F4 peripheral library was also included in the project as its nature requires some files to be modified in order to
 * suit the application usage.
 *
 * - FreeRTOS version: 8.0.0
 * - STM32F4xx DSP and Standard Peripherals Library version: 1.3.0
 *
 * Other tools
 * -----------
 * In general many very different tools ranging from hex editor to multimeter were used in development of the framework.
 * Brands and versions of most are not important enough to be noted here. The two tools where version can be important are
 * Eclipse development environment and On-Chip debugger software which was used to debug and flash the application on the
 * STM32F4 development board.
 *
 * - Eclipse SDK version 4.3.2 (Kepler Service Release 2)
 * - Eclipse CDT plug-in version: 8.3.0
 * - Eclipse CDT plug-in C/C++ GDB Hardware Debugging version: 8.3.0
 * - OpenOCD version 0.8.0 with USB support
 *
 * Provided project layout and drivers
 * -----------------------------------
 * In order to speedup development a project called beeclickarm was used as a base for the implementation. The
 * beeclikcarm project focuses on exposing STM32F4 sensors to a application running on general computer via serial
 * link. Slightly modified versions of some drivers and build script were taken from this project and incorporated into the
 * test application. The original project can be found at
 * [Github](https://github.com/bures/beeclickarm/tree/master/beeclickarm).
 *
 * Application setup
 * -----------------
 * The CDEECo++ application is expected to follow template described in usage example.
 *
 * In general the user is expected to implement CDEECO::Radio interface and create instance of it. Then the user
 * should instantiate CDEECO::System with the provided radio implementation. Once the system object is created then
 * the DEECo components inheriting from CDEECO::Component can be instantiated. The CDEECO::KnowledgeChache
 * instances may also be instantiated and passed to the system object in order to provide caching of remote knowledge. Once
 * the caches and components are in place the ensemble implementations inheriting from CDEECO::Ensemble can be
 * instantiated as their constructor requires reference to caches and components. As the last step user program is expected
 * to start FreeRTOS scheduler. Starting the scheduler will start the tasks defined by CDEECo++ system.
 *
 * Deployment
 * ----------
 * The deployment of application and the framework is the same as common deployment of embedded C/C++ application. It
 * consists from compiling the application and framework sources using cross-compiler for target architecture. Linking
 * objects together and creating single application image. Once the binary image is ready it is flashed into the embedded
 * hardware memory.
 * The deployment of the example project on the STM32F4 board is performed using make program. The configuration
 * Makefile is placed in the project root. The makefile may need to adjust tools names and paths. The defaults are set to:
 *
 * - OPENOCD=openocd
 * - CC=armv7m-hardfloat-eabi-gcc
 * - CXX=armv7m-hardfloat-eabi-g++
 * - OBJCOPY=armv7m-hardfloat-eabi-objcopy
 * - SIZE=armv7m-hardfloat-eabi-size
 *
 * Once the paths are set the project can be complied, linked and flashed to the connected STM32F4 board by issuing
 * these commands in the project's root directory:
 *
 * + $ make clean
 * + $ make
 * + $ make flash
 *
 * In order to speedup development the project can be built in parallel. The parallel build was tested with 9 tasks and
 * produced significantly faster builds on 8 core CPU. The AMD FX8350 CPU was able to build the project in 2 seconds when
 * building in parallel but it took over 12 seconds to build the project using single thread. In order to run 9 parallel
 * build threads these commands may be used:
 *
 * - $ make clean
 * - $ make -9
 * - $ make flash
 *
 * Structure of a CDEECo application
 * =================================
 *
 * CDEECo++ system structure
 * -------------------------
 * Structure of the CDEECo++ application is important for understanding the background of the user defined
 * components and ensembles. The key part of the CDEECo++ application is the system class. Its instance is used as glue
 * between components and radio interface. The system object is created with provided instance of CDEECO::Radio
 * implementation which provides the system with communication capabilities. The system object allows components to
 * broadcast their knowledge and processes knowledge fragments received from other nodes.
 * The system object also handles rebroadcasting of received knowledge fragments. In order to do that the system has
 * internal rebroadcast storage of the size defined by template argument.
 * The system also has several slots for knowledge caches. The exact number is also defined by system template argument.
 * Knowledge caches are similar to fragment caches, but they do not store knowledge fragments, but instead try to
 * reconstruct complete knowledge data from remote nodes. Each cache reconstructs knowledge of selected type which is
 * together with cache size set by template argument. Knowledge caches are used as remote knowledge sources for ensembles.
 * Thus every node has knowledge caches for knowledge types that are used by local ensembles. System stores knowledge data
 * into knowledge caches using references to caches which are set using the system's registerCache method.
 * Components are provided with reference to system which they use to broadcast knowledge changes. Ensembles are
 * instantiated with component and knowledge cache references as they use component for knowledge update and knowledge
 * cache as source of remote knowledge.
 *
 * ### Implementing radio
 * One of the things that the framework cannot do for the user is the low level communication. As there are various
 * drivers and communication media the framework cannot come with the complete implementation of driver and communication
 * handling. The CDEECo++ framework deals with packet generation, rebroadcast and processing of received data
 * using the provided radio implementation.
 * User defined radio implementation is a class that inherits from CDEECO::Radio. The CDEECO::Radio is a simple
 * class that provides radio wrapper. It manages registering a
 * receiver object which will process incoming knowledge fragments. The implementation is expected to call base class
 * receiveFragment method when new data is received and implement broadcastFragment virtual method in order
 * to allow system broadcasting new data.
 *
 * ### System and caches
 * Setting up system object and caches do not require implementation of any classes. Instead the needed objects are
 * created using the templates. As the whole application should not rely on dynamic allocation, sizes of internal data
 * needs to be set using templates. The system object has to be provided with two sizes. The first one is the maximum
 * number of knowledge caches that can be plugged in. These are used when the knowledge fragments received by radio
 * in order to store the fragment. The second one is the size of rebroadcast storage. The rebroadcast storage is used to
 * store all received knowledge fragments and rebroadcast them later. In order to do so the rebroadcast storage in the
 * system object runs a FreeRTOS thread.
 * The cache needs to be registered with the system in order to receive data. It makes no point to create cache and not
 * to register it. It is advised to create and register all caches before starting the system by running the
 * FreeRTOS scheduler.
 *
 * Implementing component
 * ----------------------
 * Component implementation includes definition of component knowledge type, knowledge trait, component class and
 * classes of component processes. The component is just normal class as well as
 * knowledge is a plain structure and processes are also normal classes. Thus the implementation can vary a lot based on
 * the user demands. The following text should be taken as recommendation and source of examples.
 *
 * ### Naming conventions
 * It is recommended to wrap a component related classes in the name-space. Let say a component named submarine is being
 * defined. The component knowledge type may be named SumbmarineKnowledge and the component class may be named
 * SubmarineComponent. Normally this would not be a problem, but the CDEECo++ framework classes are almost
 * all templates and the knowledge type will be repeated many times. Thus it is wise to create name-space Submarine
 * and define knowledge named Knowledge and the component named Component inside it. Also the processes can
 * be defined inside the name-space to simplify things.
 *
 * ### Knowledge definition
 * The knowledge is just an ordinary C++ structure, but it is handled in special way. The knowledge data are broadcast
 * as contained in the memory. Thus keeping pointers and references in the knowledge makes no sense. Instead all data
 * stored in the knowledge should be direct parts of the knowledge. Thus the knowledge memory region contains all knowledge
 * information and has constant size. Moreover when the knowledge is broadcast it has to be split into a knowledge
 * fragments as it may not fit into a packet. In order to keep the system robust and decentralized the
 * fragments created by single knowledge broadcast are not combined into a knowledge on the receiver. Instead they are
 * considered to be binary patches that can be applied on the old knowledge with the same type and id. This leads to
 * possible inconsistencies as some of the packets may be lost or delayed. This may result in partial update of the old
 * data.
 * In order to face the inconsistency the user has two options. The first one is to keep knowledge definition simple and
 * handle the inconsistencies manually. The second one is to define a knowledge trait which tells how the knowledge is
 * broken in the fragments. This approach do not solve consistency problems completely, but allows the user to keep small
 * portions of the knowledge which fits into the packet consistent. For instance this can be used to keep position
 * components consistent with each. In other words to be sure that the longitude is consistent with the latitude.
 * The control on the breaking knowledge into packets is provided by definition of the allowed packet offsets. The user
 * is supposed to define a array which contains allowed offsets in the knowledge where packets can start. When the array is
 * empty then the packet creation is not restricted. On the other side when the array is not empty then the user is
 * responsible for definition of enough offsets to allow broadcasting of the complete knowledge. If the user fails to do so
 * runtime errors may emerge. The definition of the offsets array is performed by specialization of the
 * CDEECO::KnowledgeTrait template for the target knowledge type. Unfortunately doing so is quite a lot of code and
 * the specialization must be part of the CDEECO name-space. When the template is not specialized a general version
 *  of the template is used. It contains empty array which means that the packets can start everywhere.
 * As the processes and ensembles take many template arguments and the knowledge member types are among them it is wise
 * to typedef knowledge member types in the knowledge structure.
 * For example when an ensemble which outputs the depth member of the submarine knowledge is
 * defined as an int it will be used as one of ensemble's template arguments. Among other arguments it may not be
 * clear what the int means. When a typedef is used for the depth (as in the example) then the int can
 * be replaced by Submarine::Knowledge::Depth which tells clearly what it is.
 *
 * When the knowledge needs to be initialized with certain startup values or the knowledge needs to be filled with zeros
 * at startup then this should be done in the component constructor.
 *
 * ### Periodic task definition
 * A periodic task is implemented by inheriting from CDEECO::PeriodicTask which is a template. The PeriodicTask
 * template has two template arguments. The first one is the component's knowledge type and the second is the output
 * knowledge member type. These arguments are needed in order to provide type safety for the run method. The run method
 * is virtual in the base class and needs to be implemented by user. The run method receives constant copy of the
 * knowledge as parameter and is expected to return output knowledge. When the base class is constructed it needs to be
 * provided with task period, component reference and output knowledge reference. The period will be in most cases provided
 * as constant value. Component reference is reference to the task's component. It is expected to be passed to user defined
 * task constructor when the system is being setup. The output knowledge reference is just reference to knowledge member in
 * the component's knowledge.
 * Under some conditions it may come handy to create periodic task (or triggered task) which has no output. The
 * run method would return void. This is possible by setting void as template argument that stands for
 * the output knowledge type. Then the run method will be declared as void and the base class constructor
 * will not take output knowledge reference as parameter. It may seem that it makes no sense to have tasks with no output,
 * but it may come handy when the task performs hardware control instead of pure knowledge processing.
 *
 * ### Triggered task definition
 * Definition of a triggered task is very similar to the periodic task. User defined triggered task implementation
 * inherits from the CDEECO::TriggeredTask. The base class is also a template and besides a trigger knowledge the
 * arguments are the same as for the periodic task. The triggered task differs from periodic task by extra template
 * argument that specifies type of the knowledge member used to trigger task execution. Also the triggered task constructor
 * do not require period, but instead a reference to the trigger knowledge member must be provided.
 *
 * ### Other tasks
 *
 * The current implementation provides only periodic and triggered task. Moreover the triggered tasks can react only on
 * the knowledge change. The framework itself implement the base task in a separate class CDEECO::Task. It is
 * possible to inherit from this class and create customized versions of periodic and triggered tasks or introduce a whole
 * new task concept.
 *
 * ### Task execution
 * When a task is executed the knowledge access lock is acquired, the knowledge is copied and the lock is released
 * again. Then the task is executed with the copied knowledge as constant input. When task finishes the knowledge access
 * lock is acquired again, the knowledge is updated with the task output and the lock is released. Thus the task should be
 * guaranteed not to work with partially updated knowledge.
 * Periodic and triggered task base class constructors have two more parameters that have default values set and are not
 * discussed in triggered task nor in periodic task description. These are used to set execution priority and stack size of
 * the task. Default values are set to FreeRTOS wrapper default stack size and priority which are
 * priority level 1 and 1024 bytes for stack. Overriding these default constructor values can be used to set different
 * priority or another stack size.
 *
 * Implementing ensemble
 * ---------------------
 * Similar to components and tasks an ensemble is implemented by inheriting from its base class template. In case of an
 * ensemble it is CDEECO::Ensemble template. Unfortunately ensemble template arguments are quite many. Ensemble
 * works with two components of different type so it needs to know types of their knowledge. It also has two mapping
 * functions so it needs to have two output types specified. Thus ensemble template has four template arguments. First pair
 * is formed by coordinator knowledge type and coordinator output type. The second pair is formed by member knowledge type
 * and member output type. Similarly to tasks the output type can be specified as void which disables the output.
 * This can be used to achieve one way only mapping. As there are many template arguments and the type is frequently used
 * when inheriting from template, it is recommended to typedef custom ensemble type.
 * The ensemble base class as well as the implemented ensemble class has two constructors. One is used on the
 * coordinator node in order to provide mapping from member to coordinator and the other one is used on the member node
 * where the mapping is from coordinator to member. In both cases four parameters are provided to the base class
 * constructor. The first one is pointer to the component. The second one is pointer to the output member of the
 * component's knowledge. The third one is pointer to the library of the remote knowledge which is periodically scanned for
 * possible knowledge exchange candidates. The last one is the exchange execution period. As well as in case of processes
 * an ensemble is free to store some user defined values in the ensemble class. These will not be affected by the
 * framework.
 *
 * Public API
 * ----------
 * Beside the classes and methods discussed in the previous parts there are some methods that can extend usage of the
 * framework. Tasks and ensembles are restricted to output only one value. It is possible to contain more output values to
 * substructure and then output that structure. This is how position is returned as structure containing longitude and
 * latitude in the examples. Unfortunately sometimes the knowledge cannot be restructured in this way. In such cases
 * component's methods lockReadKnoweldge and especially lockWriteKnowledge may be handy. These two are used
 * by the framework when a task is executed. The first one is used to copy the task input and the second is sued to write
 * task output. Using the second one the user can output more values in the task at the cost of losing synchronization
 * between writes. It may happen that the two subsequent calls to lockWriteKnowledge will be split by some other
 * call to the same method in another task or ensemble.
 * The component class also contains methods getId and getType which may also be handy in some situations.
 *
 * Implementation details
 * ======================
 * This part reveals framework internal structures and principles. It should be consulted when the framework is about to
 * be extended or functionality modified. It also discusses collection of drivers that were included in order to provide
 * hardware access for the example code.
 * The drivers are implemented as C++ classes. They are designed to be used as global objects due to need of calling
 * their methods from interrupt servicing routines.
 *
 * Drivers
 * -------
 * Beside console driver the drivers are not used by the framework. Their purpose is to accompany example usage of the
 * framework. The drivers reside in the src/drivers directory.
 *
 * ### SHT1x
 * This driver was implemented from scratch in order to provide sensor access in the example application code. It
 * provides control over SHT1x temperature and humidity sensor. The SHT1x chip has many features, but this
 * driver aims to provide just basic access. It allows user to read temperature and humidity using the highest precision
 * without CRC control.
 *
 * ### GMD1602
 * The GMD1602 driver provides interface to 16x2 character alphanumerical display using 4 data lines. It was
 * implemented as practice in the low level hardware control and used in early project stages. Unfortunately the nature of
 * the device and lack of configuration in the driver prevent usage of the device with this driver once the shield with
 * additional hardware is connected to the STM32F4 board.
 *
 * ### Console
 * Console is not a true driver. In fact it is front-end to the UART driver and provides logging of
 * messages to the serial port connected via USB to the PC. The serial link can be used as both great debugging
 * tool and communication channel to reveal DEECo information to other parts of the application. The console is the only
 * driver used directly by the framework. The current implementation of the framework to console binding requires quite a
 * bit of user cooperation when including header files. Fortunately the console usage in the framework is limited just to
 * logging so the console usage can be easily removed from the framework.
 *
 * ### StopWatch
 * The StopWatch driver is not intended for general usage. Instead it is designed to be used for execution time
 * measurements at the microsecond level. It has very short maximum measurement period but when used with enabled
 * interrupts, it should detect underlaying timer overruns.
 *
 * ### Provided drivers
 * Various drivers were included in the project that were provided as a base for implementation of this thesis.
 * These drivers include Button driver, GPS driver, LED
 * drivers, MRF24J40 radio driver, Timer and UART driver.
 *
 * FreeRTOS integration
 * --------------------
 * The FreeRTOS is a C library so it has C language API. In order to simplify usage of the
 * FreeRTOS features in the framework a set of C++ wrapping classes were added to the project. These are placed
 * in the src/wrappers directory and include wrappers for FreeRTOS mutex, FreeRTOS semaphore
 * and FreeRTOS task. Wrappers are simple classes that hold the wrapped object handle and provide methods that
 * wrap the related FreeRTOS function calls. Some less frequent FreeRTOS functions are called
 * directly from the framework without the wrappers.
 * As the FreeRTOS allocates memory when an task or semaphore is created it needs a memory allocator.
 * FreeRTOS is quite flexible with allocator selection. There is option to use own allocator or to use some
 * implementation provided by FreeRTOS itself. It was decided to use provided allocator that internally uses
 * malloc and free provided by C runtime. This approach has been tested to work the best of provided
 * implementations. There are concerns about the real-time properties of the system as the malloc and free
 * calls may block execution in critical sections. Fortunately the whole system can work such way that it allocates only at
 * startup and never call free. Thus it will never allocate from fragmented heap.
 * Features provided by FreeRTOS are used for creation of processes/threads found in components and ensembles
 * as well as for internal tasks inside the framework. Beside task management functions, synchronization primitives
 * provided by FreeRTOS, are heavily used by the framework.
 *
 * Remote knowledge management
 * ---------------------------
 * There are two subsystems that handle the remote knowledge in the framework. The first one is responsible for
 * rebroadcasting of received knowledge fragments. The second one is used to store fragments of defined types and combine
 * them into a complete knowledge. Complete knowledge records are used as source of remote component knowledge for
 * ensembles.
 *
 * ### Knowledge fragment rebroadcasting
 * Rebroadcasting of received fragment is performed by RebroadcastStorage class. It is a template the only
 * argument of which is the size of the rebroadcast storage. The class contains static array of RebroadcastRecord
 * type. Each record contains knowledge fragment data, received time-stamp, scheduled rebroadcast time-stamp and used flag.
 * When new knowledge fragment is received it is added to the storage with certain probability in order to implement
 * stochastic time-to-live. If there is no free slot in the storage the oldest record is rebroadcast and replaced by new
 * one. When new record is added a rebroadcast time-stamp is calculated based on the received link quality. The calculation
 * is quite simple. Rebroadcast interval is linear function of receiver link quality. Both stochastic time-to-live and
 * rebroadcast interval calculation may need further tunning to adapt real environment with more deployed nodes.
 *
 * RebroadcastStorage class has internal thread which is used to periodically check storage for records to be
 * rebroadcast. Access to the storage is protected by mutex. Possible delay causes by waiting on the mutex is limited by
 * rebroadcast storage size and generally do not cause problems.
 *
 * ### Typed knowledge cache
 * Unlike rebroadcast cache the knowledge cache is a bit more complicated. The actual storage is implemented by class
 * KnowledgeCache which is also template. It takes three template arguments. The first one specifies component type
 * magic number. The second one is the knowledge type. The last one is size of the cache. Each type of knowledge is handled
 * by custom instance of KnowledgeCache class template.
 * The cache is also formed by fixed array of records. Each record holds: knowledge data, mask (valid regions of the
 * data), time-stamp and complete flag. Each time new knowledge fragment of matching cache type and record id is processed
 * its data are added to the record and the availability mask is updated. When the mask covers whole knowledge than the
 * complete flag is set to true. If the cache is full then the oldest record is replaced.
 * The KnowledgeCache class inherits from two helper classes. The first one is the KnowledgeStorage class
 * which is an interface for storing fragments in the cache. It is not a template thus its type can be used to store array
 * of caches in the CDEECO::System class. Instances of this type can be used to store received fragments. The second
 * one is the KnowledgeLibrary template. It has the only template argument which specifies knowledge type. The
 * library can iterate over the complete records in the cache. Thus it allows ensembles to query complete cache records for
 * membership and possible knowledge exchange. The library interface simplifies cache handling as the access to the library
 * is possible without knowing cache size and knowledge magic, but still the library has the knowledge type so it can
 * return properly typed data.
 *
 * Component
 * ---------
 * Component is represented by class template Component the only template argument of which is the knowledge
 * type. The component class is responsible for knowledge storage, access and running triggered tasks.
 * Knowledge is public member of component class and the class provides methods to safely read and write knowledge.
 * These are lockReadKnowledge which is used to obtain consistent copy of the knowledge and
 * lockWriteKnowledge which is used to consistently write part of the knowledge.
 * Triggered tasks are added using the component's method addTriggeredTask. It stores triggered tasks in the
 * linked list using members provided via ListedTriggerTask interface which is implemented by TriggeredTask
 * class. The root of the linked list is stored in the Component class itself. When the lockWriteKnowledge
 * method is executed and new knowledge is different from the old one then the listed tasks are consulted and those
 * affected are executed.
 * The component also owns a thread which is set to periodically broadcast complete component knowledge. Doing so is
 * important as broadcasting changed parts is not enough. Remote nodes can miss some rare updates of several knowledge
 * areas. Thus their cached knowledge will remain incomplete and unusable.
 *
 * Processes
 * ---------
 * A DEECo process is called task in the CDEECo++ context. Task class templates form a hierarchy
 * where the responsibilities are split. The top level templates called TriggeredTask and PeriodicTask are
 * responsible for scheduling while they inherit ability to execute the task code from base Task. The base task
 * itself is composed of the Task and its base TaskBase in order to reduce code duplication.
 * All the task related classes are templates and take knowledge type and output knowledge type as template arguments.
 * The triggered task also takes a trigger knowledge type as template argument. As the output knowledge type may be
 * void and creating references to void is not allowed the Task template has to be specialized for the
 * void output knowledge. The specialized implementation do not contain output knowledge reference, thus it do not
 * write output knowledge and avoids creating references to void. The common parts of the specialized and normal
 * implementation were moved to TaskBase in order not to duplicate the code.
 * Constructors of the task related classes take several parameters. The TriggeredTask and the
 * PeriodicTask are the only instantiated by user. Those take all parameters and pass some of them to base class
 * constructors. The period and trigger knowledge reference are used directly by PeriodicTask respective
 * TriggeredTask. Component reference and output knowledge reference (if used) are passed to the base classes. In
 * order to allow user not to pass output knowledge reference when the output type is void the top-level classes has two
 * constructors. One passes output knowledge while the other one do not. The output knowledge reference is passed from
 * constructor to base constructor as auto type reference which allows the code to be valid even when the output
 * knowledge type is void. Doing so requires usage of C++1y features.
 * The TaskBase class which is at the base of the task class hierarchy has the virtual method run. The
 * method takes constant copy of the knowledge as parameter and returns output knowledge type. The run method is not
 * implemented inside the framework as it is expected to be implemented by the user and contain task code to be executed
 * when the task runs.
 *
 * ### Periodic task
 * Periodic task is quite simple compared to triggered task. Its only parameter is the period. The PeriodicTask
 * class also inherits from FreeRTOSThread and uses the thread to perform periodic execution of the execute
 * method provided by base task.
 *
 * ### Triggered task
 * On the other side the triggered task is more complicated. It also has internal thread which runs the base class
 * execute method. The thread lowers the semaphore and execute the task in infinite cycle. The semaphore has initial
 * value of zero. So each rise of the semaphore value causes task to execute. In order to perform check whenever the
 * watched knowledge has been changed the TriggeredTask inherits from ListedTriggerTask. Listed trigger task
 * members are used to form a linked list of triggered tasks. The list is rooted in the component and check methods are
 * executed on the whole list when the knowledge is changed. As the template function cannot be virtual the checked region
 * is passed in form of two pointers that mark the area in the knowledge structure.
 *
 * Ensemble
 * --------
 * The Ensemble design is similar to two combined periodic tasks. Class Ensemble is a template which takes
 * two pairs of knowledge and knowledge output types as template arguments. The first pair is used for coordinator and the
 * second one for the member. The user implements virtual ensemble methods in order to provide membership decision method,
 * member to coordinator mapping method and coordinator to member mapping method.
 * Once the ensemble is implemented it has to be able to be used in two different ways. They can be instantiated on the
 * node where coordinator resides and map from member to coordinator. It also can be instantiated on the node where member
 * resides and thus provide mapping from coordinator to member. In order to accomplish this the ensemble base type has
 * pointers to both member and coordinator knowledge output, but just one pair is used by every instance. The
 * Ensemble has two constructors one takes coordinator component and member library, the second one takes member
 * component and coordinator library. Where the KnowledgeLibrary is interface to KnowledgeCache that provides
 * iterating over remote knowledge of specified type.
 * Similarly to the tasks the output knowledge type for either coordinator output knowledge or member output knowledge
 * may be defined as void. In case of ensembles it makes very good sense to do so as it may be desired to provide
 * just one-way mapping. Unfortunately when the template argument is set to void an illegal code occurs in the ensemble
 * template. This is caused mainly by execution of mapping functions as the code stores return in the variable and the the
 * variable cannot be declared to have void type. In order to avoid compilation errors SFIANE feature is
 * used to handle the cases where the error can occur. The resulting template code looks complicated, but the only point is
 * to mask methods that makes no sense when the particular template argument is set to void.
 * In order to run the membership tests and the knowledge exchange an ensemble inherits from the
 * FreeRTOSTask thus it contains a thread. It uses periodic scheduling to execute membership tests and
 * possibly run the knowledge exchange.
 *
 * System
 * ------
 * The system provides binding between radio and other parts of the system. It is quite simple class template. Template
 * arguments specify maximum number of caches and size of rebroadcast storage which is also hosted in the system class.
 * The system is just a proxy through which the components broadcast their knowledge fragments. It is also responsible
 * for processing received data. The received data is stored in the rebroadcast cache which is included in the
 * CDEECO::System and the received data are also passed to registered knowledge caches. The caches are
 * registered using the CDEECO::KnowledgeStorage interface. The interface hides template arguments of the
 * CDEECO::KnowledgeCache, thus it simplifies storage of pointer pointing to the registered caches.
 * As many classes in the system has template arguments which complicate their usage as those needs to be passed to
 * every other class that will use those templates it was decided to implement interfaces which hide those template
 * arguments. As the system is used to broadcast and receive the knowledge fragments it inherits from Receiver and
 * Broadcaster. These are simple interfaces which take no template arguments and can be used easily without
 * complicated template constructs. Thanks to those interfaces component template do not have to have size of rebroadcast
 * storage as argument.
 *
 * Portability to different hardware
 * ---------------------------------
 * Embedded hardware differs quite a lot when dealing with different models, brands and kinds of hardware. As the output
 * of this thesis is not a single application but a framework that is expected to be used on wide range of hardware the
 * framework was designed not to rely on particular hardware features. For instance the framework do not use floating point
 * variables, nor it talks to hardware directly. Instead it moves application specific hardware access implementation to
 * the user who has to implement sensor drivers and communication interface. The rest of the hardware specific code, namely
 * the scheduling and synchronization, was implemented using the widely used real-time operating system called
 * FreeRTOS. It was ported to 34 architectures at the time of writing. Thus porting the framework to one of
 * those architectures should be as easy as changing the configuration. Detailed FreeRTOS hardware support can
 * be found at [FreeRTOS](http://www.freertos.org).
 *
 */
