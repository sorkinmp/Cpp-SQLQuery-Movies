#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include <string>
#include <iostream>
#include "Movie.h"

/** A fixed HTML header that is printed at the beginning of output to ensure
 * the output is displayed correct.
 * Copyright (C) 2019 sorkinmp@miamioh.edu
 */
const std::string HTMLHeader = 
    "Content-Type: text/html\r\n\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "<link type='text/css' rel='stylesheet' href='movie.css'/>\n"
    "</head>\n"
    "<body>";

/** A fixed HTML footer that is printed at the end of output to ensure
 * correct HTML formatting
 */
const std::string HTMLFooter = "</body>\n</html>";

/** Convenience method to decode HTML/URL encoded strings.

    This method must be used to decode query string parameters
    supplied along with GET request.  This method converts URL encoded
    entities in the from %nn (where 'n' is a hexadecimal digit) to
    corresponding ASCII characters.

    \param[in] str The string to be decoded.  If the string does not
    have any URL encoded characters then this original string is
    returned.  So it is always safe to call this method!

    \return The decoded string.
*/
std::string url_decode(std::string str) {
    // Decode entities in the from "%xx"
    size_t pos = 0;
    while ((pos = str.find_first_of("%+", pos)) != std::string::npos) {
        switch (str.at(pos)) {
            case '+': str.replace(pos, 1, " ");
            break;
            case '%': {
                std::string hex = str.substr(pos + 1, 2);
                char ascii = std::stoi(hex, nullptr, 16);
                str.replace(pos, 3, 1, ascii);
            }
        }
        pos++;
    }
    return str;
}

/** A helper method to process string inputs.
 * 
 * @param title The title of the movie.
 * @param genres The genres of the movie
 */
void titleGenreProcess(std::string& title, std::string& genres) {
    // decode the string
    title = url_decode(title);
    genres = url_decode(genres);
}

/** A helper method to turn year hexadecimal into integer values.
 * 
 * @param value3 The hexadecimal value of startYear.
 * @param value4 The hexadecimal value of endYear.
 * @param startYear The int value of startYear.
 * @param endYear The int value of endYear.
 */
void yearProcess(std::string& value3, std::string& value4, int& startYear,
                int& endYear) {
    // do empty checks
    if (value3.length() > 0) {
        startYear = stoi(url_decode(value3));
    }
    // make sure to remove '\n' from value4
    if (value4.length() > 0) {
        endYear = stoi(url_decode(value4));
    }
}

/** A helper method to print out the SQL query result.
 * 
 * @param result The SQL query results to be printed.
 */
void printQuery(const mysqlpp::StoreQueryResult& result) {
    Movie m;
    std::cout << HTMLHeader << std::endl;
    // use for loop
    for (size_t row = 0; row < result.size(); row++) {
        m.setTitle(result[row][0].c_str());
        m.setYear(result[row][1]);
        m.setGenres(result[row][2].c_str());
        m.setRating(result[row][3]);
        m.setNumRaters(result[row][4]);
        m.setImdbID(result[row][5]);
        m.setID(result[row][6]);
        m.printAsHtml(std::cout);
    }
    // All done
    std::cout << HTMLFooter << std::endl;
}

/** A print method to print out a query that only has title or genre or both
 *  as the bind variables.
 * 
 * @param title The title bind variable of the movie object.
 * @param genre The genre bind variable of the movie object.
 */
void printNoYearQuery(const std::string& title, const std::string& genre) {
    // Connect to database with: database, server, userID, password
    mysqlpp::Connection myDB("cse278s19", "os1.csi.miamioh.edu", "cse278s19",
            "rbHkqL64VpcJ2ezj");
    // Create a query
    mysqlpp::Query query = myDB.query();
    if (title.length() == 0) {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE genres LIKE '%%%0%%';";
        query.parse();  // check to ensure query is correct
        // Run the query and get stored results
        mysqlpp::StoreQueryResult result = query.store(genre);
        // Print results from query as an HTML table
        printQuery(result);
    } else if (genre.length() == 0) {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE title LIKE '%%%0%%';";
        query.parse();  // check to ensure query is correct
        // Run the query and get stored results
        mysqlpp::StoreQueryResult result = query.store(title);
        // Print results from query as an HTML table
        printQuery(result);
    } else {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE title LIKE '%%%0%%' and genres LIKE '%%%1%%';";
        query.parse();  // check to ensure query is correct
        // Run the query and get stored results
        mysqlpp::StoreQueryResult result = query.store(title, genre);
        // Print results from query as an HTML table
        printQuery(result); }
    // printQuery(result);
}

/** A print method to print out a query that only contains either just startYear
 *  as the bind variable or both startYear and title as bind variables.
 * 
 * @param title The title bind variable of the movie object.
 * @param startYear The startYear bind variable of the movie object.
 */
void printStartYearOnlyQuery(const std::string& title, const int& startYear) {
    // Connect to database with: database, server, userID, password
    mysqlpp::Connection myDB("cse278s19", "os1.csi.miamioh.edu", "cse278s19",
            "rbHkqL64VpcJ2ezj");
    // Create a query
    mysqlpp::Query query = myDB.query();
    if (title != "") {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE title LIKE '%%%0%%' and year >= %1;";
        query.parse();
        mysqlpp::StoreQueryResult result = query.store(title, startYear);
        printQuery(result); 
    } else {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE year >= %0;";
        query.parse();
        mysqlpp::StoreQueryResult result = query.store(startYear);
        printQuery(result);
    }
}

/** A print method to print out a query that only contains either just endYear
 *  as the bind variable or both endYear and title as bind variables.
 * 
 * @param title The title bind variable of the movie object.
 * @param endYear The endYear bind variable of the movie object.
 */
void printEndYearOnlyQuery(const std::string& title, const int& endYear) {
     // Connect to database with: database, server, userID, password
    mysqlpp::Connection myDB("cse278s19", "os1.csi.miamioh.edu", "cse278s19",
            "rbHkqL64VpcJ2ezj");
    // Create a query
    mysqlpp::Query query = myDB.query();
    // check title field
    if (title != "") {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE title LIKE '%%%0%%' and year <= %1;";
        query.parse();
        mysqlpp::StoreQueryResult result = query.store(title, endYear);
        printQuery(result); 
    } else {
    query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE year <= %0;";
    query.parse();
    mysqlpp::StoreQueryResult result = query.store(endYear);
    printQuery(result);
    }
}

/** A print method to print out a query that either contains both startYear and
 *  endYear as the bind variables or title, genre, startYear, and endYear as the
 *  bind variables.
 * 
 * @param title The title bind variable of the movie object.
 * @param genre The genre bind variable of the movie object.
 * @param startYear The startYear bind variable of the movie object.
 * @param endYear The endYear bind variable of the movie object.
 */
void printBothYearQuery(const std::string& title, const std::string& genre,
                   const int& startYear, const int& endYear) {
    // add in if title, genre contained too
     // Connect to database with: database, server, userID, password
    mysqlpp::Connection myDB("cse278s19", "os1.csi.miamioh.edu", "cse278s19",
            "rbHkqL64VpcJ2ezj");
    // Create a query
    mysqlpp::Query query = myDB.query();
    // do title and genre check to see if they are bind variables
    if (title != "" && genre != "") {
        query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE title LIKE '%%%0%%' and genres LIKE '%%%1%%' and "
                "year >= %2 and year <= %3;";
        query.parse();  // check to ensure query is correct
        // Run the query and get stored results
        mysqlpp::StoreQueryResult result = query.store(title, genre, 
                                                       startYear, endYear);
        // Print results from query as an HTML table
        printQuery(result);
    } else {
    query << "SELECT title, year, genres, rating, raters, imdb_id, id  "
              <<  "FROM Movies "
              << "WHERE year <= %0 && year >= %1;";
    query.parse();  // check to ensure query is correct
    // Run the query and get stored results
    mysqlpp::StoreQueryResult result = query.store(startYear, endYear);
    // Print results from query as an HTML table
    printQuery(result);
    }
}

/** The main method that processes CGI input data from standard input
 * The input will be in the format: 
 * title=M%61tr%69x&genre=&startYear=&endYear=
 * Ensure you URL decode each value prior to using it.
 *
 * Tip: Using std::getline(std::cin, param1, '=') and
 * std::getline(std::cin, value1, '&') is handy to read parameter and
 * value pairs that are separated by '=' and '&' characters.
 */
int main() {
    // create variables
    std::string titleIn, value1, genreIn, value2, startyear, value3, 
                endYearIn, value4;
    int startYear = -1;
    int endYear = -1;
    // get titleIn
    std::getline(std::cin, titleIn, '='); std::getline(std::cin, value1, '&');
    // get genreIn
    std::getline(std::cin, genreIn, '='); std::getline(std::cin, value2, '&');
    // get startyear
    std::getline(std::cin, startyear, '='); std::getline(std::cin, value3, '&');
    // get endYearIn
    std::getline(std::cin, endYearIn, '='); 
    std::getline(std::cin, value4, '\n');
    // obtain url_decode values for title, genre, startYear, endYear
    titleGenreProcess(value1, value2);
    yearProcess(value3, value4, startYear, endYear);
    // value1 = title, value2 = genres, value3 = startYear, value4 = endYear
    // check to see which method to run
    // if both startYear and endYear have values
    if (startYear != -1 && endYear != -1) {
        printBothYearQuery(value1, value2, startYear, endYear);
    } else if (startYear == -1 && endYear != -1) {
        // if only endYear has a value
       printEndYearOnlyQuery(value1, endYear);
    } else if (startYear != -1 && endYear == -1) {
        // if only startYear has a value
        printStartYearOnlyQuery(value1, startYear);
    } else {
        // if both startYear and endYear have no values
        printNoYearQuery(value1, value2);
    }
    return 0;
}
