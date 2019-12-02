#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <math.h>
#include <set>

#define mine_d std::vector<std::vector<std::vector<Loc<uint8_t>>>>

template <class T>
class Loc
{
    T           x;
    T           y;
    T           z;
    std::string block;
public :

    Loc(){};

    Loc(Loc const &loc) : Loc(loc.x, loc.y, loc.z){}

    Loc(T x, T y, T z, std::string block) : block(block), x(x), y(y), z(x){}

    Loc(T x, T y, T z):x(x),y(y),z(z){}

    T       getX()
    {
        return x;
    }

    T       getY()
    {
        return y;
    }

    T       getZ()
    {
        return z;
    }

    void    setX(T x)
    {
        this->x = x;
    }

    void    setY(T y)
    {
        this->y = y;
    }

    void    setZ(T z)
    {
        this->z = z;
    }

    std::string get_block()
    {
        return block;
    }

    void set_block(std::string block)
    {
        this->block = block;
    }

    template <class E>
    inline uint16_t get_distance(Loc <E>point_2)
    {
       return sqrt((this->x - point_2.getX()) * (this->x - point_2.getX()) +
                   (this->y - point_2.getY()) * (this->y - point_2.getY()) +
                   (this->z - point_2.getZ()) * (this->z - point_2.getZ()));
    }
};

uint8_t get_interval_i(int16_t a, int16_t b)
{
    if (a - b > 0)
        return (uint8_t) (a - b);
    else
        return (uint8_t) (b - a);
}

class Reg
{
    Loc <int16_t >max_corner;
    Loc <uint8_t >interval;
public :

    Reg(const Loc <int16_t>corner, const Loc <uint8_t >interval) : max_corner(corner), interval(interval){}

    Reg(Loc <int16_t >corner_1, Loc <int16_t >corner_2) :
    max_corner(Loc<int16_t>
    (
                    corner_1.getX() < corner_2.getX() ? corner_1.getX() : corner_2.getX(),
                    corner_1.getY() < corner_2.getY() ? corner_1.getY() : corner_2.getY(),
                    corner_1.getZ() < corner_2.getZ() ? corner_1.getZ() : corner_2.getZ())
    ),
    interval(Loc<uint8_t>
    (
                    get_interval_i(corner_1.getX(), corner_2.getX()),
                    get_interval_i(corner_1.getY(), corner_2.getY()),
                    get_interval_i(corner_1.getZ(), corner_1.getZ()))
     ){}

     Loc<int16_t>  get_corner()
     {
        return max_corner;
     }

     Loc<uint8_t>   get_interval()
     {
        return interval;
     }

     int            get_vol()
     {
        return interval.getZ() * interval.getY() * interval.getX();
     }
};

class mine
{
    mine_d  _mine;
    uint8_t size;
    std::vector<std::vector<Loc <uint8_t>>> worms;
public:
    mine(uint8_t size, std::string block) : size(size)
    {
        mine_d  mine;
        uint8_t x;
        uint8_t y;
        uint8_t z;

        y = -1;
        while (++y < size)
        {
            std::vector<std::vector<Loc<uint8_t>>> plane;
            x = -1;
            while (++x < size)
            {
                std::vector<Loc<uint8_t>> line;
                z = -1;
                while (++z < size)
                {
                    line.push_back(Loc<uint8_t>(x, y, z, block));
                }
                plane.push_back(line);
            }
            mine.push_back(plane);
        }
        _mine = mine;
    }

    Loc     <uint8_t >get_midpoint(Loc <uint8_t> point_1, Loc <uint8_t> point_2, float intensive)
    {
        uint8_t vector[3];
        char    i;
        uint8_t distance;

        vector[0] = get_interval_i(point_1.getX(), point_2.getX());
        vector[1] = get_interval_i(point_1.getY(), point_2.getY());
        vector[2] = get_interval_i(point_1.getZ(), point_2.getZ());

        distance = sqrt(vector[0] * vector[0] +
                        vector[1] * vector[1] +
                        vector[2] * vector[2]);

        if (distance < 3)
            return Loc<uint8_t>(0, 0, 0);

        intensive = distance * intensive > 1.0 ? distance * intensive : 1;

        int8_t buf;
        i = -1;
        while (++i < 3)
        {
            while (true)
            {
                buf = vector[i] / 2 + (rand() % (int)intensive) - (int)intensive / 2;
                if (buf >= 0 && buf < size)
                    break ;
            }
            vector[i] = buf;
        }

        vector[0] += point_1.getX() < point_2.getX() ? point_1.getX() : point_2.getX();
        vector[1] += point_1.getY() < point_2.getY() ? point_1.getY() : point_2.getY();
        vector[2] += point_1.getZ() < point_2.getZ() ? point_1.getZ() : point_2.getZ();

        return Loc<uint8_t>(vector[0], vector[1], vector[2]);
    }

    void add_points_dis(Loc <uint8_t>point_1, Loc <uint8_t>point_2, std::vector<Loc <uint8_t>> *worm, u_char dis, float intensive)
    {
        if (point_1.get_distance(point_2) < dis)
            return ;
        auto point_1_2 = get_midpoint(point_1, point_2, intensive);

        //std::cout << point_1.get_distance(point_2) << " - " << point_1_2.get_distance(point_1) << std::endl;

        worm->push_back(point_1_2);
        add_points_dis(point_1, point_1_2, worm, dis, intensive);
        add_points_dis(point_2, point_1_2, worm, dis, intensive);
    }

    void set_tunnel(uint8_t rx, uint8_t ry, uint8_t rz)
    {
        static double dia = 0;
        static int c = 0;

        Loc <uint8_t>point_1;
        Loc <uint8_t>point_2;

        int start_plane = rand() % 3;

        point_1.setX(rand() % (size - rx * 2) + rx);
        point_1.setY(rand() % (size - ry * 2) + ry);
        point_1.setZ(rand() % (size - rz * 2) + rz);

        point_2.setX(rand() % (size - rx * 2) + rx);
        point_2.setY(rand() % (size - ry * 2) + ry);
        point_2.setZ(rand() % (size - rz * 2) + rz);

        if (start_plane == 0)
        {
            point_1.setX(rx);
            point_2.setX(size - rx);
        }
        else if (start_plane == 1)
        {
            point_1.setZ(rz);
            point_2.setZ(size - rz);
        }
        else if (start_plane == 2)
        {
            point_1.setY(ry);
            point_2.setY(size - ry);
        }

        std::vector<Loc <uint8_t>> *worm = new std::vector<Loc <uint8_t>>();

        worm->push_back(point_1);
        worm->push_back(point_2);

        add_points_dis(point_1, point_2, worm, 5, 0.5);

        std::cout << worm->size() << std::endl;

        std::sort(worm->begin(), worm->end(), [](Loc <uint8_t> a1, Loc <uint8_t> a2) {
            if (a1.getX() != a2.getX())
                return a1.getX() < a2.getX();
            if (a1.getY() != a2.getY())
                return a1.getY() < a2.getY();
            return a1.getZ() < a2.getZ();
        });

        int8_t x;
        int8_t y;
        int8_t z;

        std::vector<Loc<uint8_t>> *big_worm = new std::vector<Loc <uint8_t>>();

        static int count = 0;

        for (auto block : *worm)
        {
            x = -rx;
            while (++x < rx)
            {
                y = -ry;
                while (++y < ry)
                {
                    z = -rz;
                    while (++z < rz)
                    {
                        std::cout << (x * x) / ( 1.0 * (rx * rx)) + (y * y) / ( 1.0 * (ry * ry)) + (z * z) / ( 1.0 * (rz * rz)) << " " << 1 + (rand() % 100 - 50) / 100.0 << std::endl;
                        if ((x * x) / ( 1.0 * (rx * rx)) + (y * y) / ( 1.0 * (ry * ry)) + (z * z) / ( 1.0 * (rz * rz)) < 1 + (rand() % 100 - 50) / 100.0 &&
                        x + block.getX() > 0 &&
                        x + block.getX() < size &&
                        y + block.getY() > 0 &&
                        y + block.getY() < size &&
                        z + block.getZ() > 0 &&
                        z + block.getZ() < size)
                            _mine[x + block.getX()][y + block.getY()][z + block.getZ()].set_block("air");
                    }
                }
            }
        }

        worms.push_back(*worm);

        x = -1;
        while (++x < size)
        {
            y = -1;
            while (++y < size)
            {
                int pr_in = false;
                z = -1;
                while (++z < size)
                {
                    if (_mine[x][y][z].get_block() == "air")
                    {
                        if (pr_in != 9)
                        pr_in++;
                    }
                }
                if (pr_in)
                    std::cout << pr_in;
                else
                    std::cout << '.';
            }
            std::cout << "    ";

            y = -1;
            while (++y < size)
            {
                int pr_in = false;
                z = -1;
                while (++z < size)
                {
                    if (_mine[z][x][y].get_block() == "air")
                    {
                        if (pr_in != 9)
                            pr_in++;
                    }
                }
                if (pr_in)
                    std::cout << pr_in;
                else
                    std::cout << '.';
            }
            std::cout << "    ";

            y = -1;
            while (++y < size)
            {
                int pr_in = false;
                z = -1;
                while (++z < size)
                {
                    if (_mine[y][z][x].get_block() == "air")
                    {
                        if (pr_in != 9)
                            pr_in++;
                    }
                }
                if (pr_in)
                    std::cout << pr_in;
                else
                    std::cout << '.';
            }
            std::cout << '\n';
        }

        delete worm;
    }
};

inline int16_t get_rand(int interval, double flag)
{
    return rand() % interval - interval * flag;
}

Reg    get_in(std::vector<Reg> list, Loc<int16_t> block, bool get_stat)
{
    static long count = 0;

    if (get_stat)
    {
        std::cout << count << std::endl;
        return Reg(Loc<int16_t>(0, 0, 0), Loc<int16_t>(0, 0, 0));
    }
    int interval;
    for (auto reg : list)
    {
        count++;
        interval = block.getX() - reg.get_corner().getX();
        if (interval >= 0 && interval <= reg.get_interval().getX())
        {
            interval = block.getZ() - reg.get_corner().getZ();
            if (interval >= 0 && interval <= reg.get_interval().getZ())
            {
                interval = block.getY() - reg.get_corner().getY();
                if (interval >= 0 && interval <= reg.get_interval().getY())
                {
                    std::cout << "f: " << count << "\n";
                    return reg;
                }
            }
        }
    }
    return Reg(Loc<int16_t>(0, 0, 0), Loc<int16_t>(0, 0, 0));
}

int main()
{
    int size = 80;

    while (size != 1)
    {
        std::cin >> size;
        auto first = std::chrono::high_resolution_clock::now();
        mine m(size, "stone");
        std::cin >> size;
        int x = -1;
        while (++x < size)
        {
            //m.set_tunnel(10, 10, 10);

            m.set_tunnel(3 + get_rand(2, 0), 3 + get_rand(2, 0), 3 + get_rand(2, 0));
        }
        auto second = std::chrono::high_resolution_clock::now();
        auto timerResult = std::chrono::duration_cast<std::chrono::milliseconds>(second - first).count();
        std::cout << "all time: " << timerResult << std::endl;
    }
    return 0;
}