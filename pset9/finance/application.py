import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions, HTTPException, InternalServerError
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True


# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    user_id = session.get("user_id")
    rows = db.execute("SELECT * FROM users WHERE id=?", user_id)
    user = rows[0]

    # Retrieve all user's purchases
    purchases = db.execute("SELECT symbol, amount FROM stocks WHERE user_id=? AND amount > 0", user_id)

    total_holdings = user["cash"]

    # For each purchase made, add their current info on the purchases list
    for p in purchases:
        stock = lookup(p["symbol"])
        p["name"] = stock["name"]
        p["price"] = usd(stock["price"])
        total = stock["price"] * p["amount"]
        p["total"] = usd(total)

        # Add to total holdings
        total_holdings += total

    return render_template("index.html", purchases=purchases, cash=usd(user["cash"]), total=usd(total_holdings))


@app.route("/add-cash", methods=["GET", "POST"])
@login_required
def add_cash():
    """Add cash to account"""

    # Retrieve current user's information
    user_id = session.get("user_id")
    rows = db.execute("SELECT * FROM users WHERE id=?", user_id)
    user = rows[0]

    if request.method == "POST":
        cash = request.form.get("cash")

        try:
            amount = float(cash)
        except:
            return apology("invalid amount", 400)

        if not cash or amount < 0:
            return apology("invalid amount", 400)

        db.execute("UPDATE users SET cash = ? WHERE id=?", (user["cash"] + amount), user_id)

        return redirect("/")
    else:
        return render_template("add-cash.html", balance=usd(user["cash"]))


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Ensure a symbol was provided
        if not symbol:
            return apology("must provide symbol", 400)

        try:
            amount = int(shares)
        except:
            return apology("invalid share amount", 400)

        # Ensure a valid amount of shares was provided
        if not shares or int(shares) < 0:
            return apology("invalid share amount", 400)

        stock = lookup(symbol)

        # Make sure the provided symbol exists
        if not stock:
            return apology("invalid symbol", 400)

        # Retrieve current user's information
        user_id = session.get("user_id")
        rows = db.execute("SELECT * FROM users WHERE id=?", user_id)
        user = rows[0]

        # Ensure user has enough funds for purchase
        total_price = stock["price"] * amount

        if user["cash"] < total_price:
            return apology("not enough funds", 403)

        # Remove total amount from user's account
        db.execute("UPDATE users SET cash = ? WHERE id=?", (user["cash"] - total_price), user_id)

        # Register stock purchase
        rows = db.execute("SELECT id, amount FROM stocks WHERE user_id = ? AND symbol = ?", user_id, symbol)
        # current = rows[0]

        if len(rows) == 0:
            current_id = db.execute("INSERT INTO stocks (user_id, name, symbol, amount) VALUES (?, ?, ?, ?)",
                                    user_id, stock["name"], symbol, amount)
        else:
            current = rows[0]
            current_id = current["id"]
            db.execute("UPDATE stocks SET amount = ? WHERE user_id = ? AND symbol = ?",
                       (current["amount"] + amount), user_id, symbol)

        # Save purchase in history
        db.execute("INSERT INTO history (user_id, stock_id, price, amount, action, date) VALUES (?, ?, ?, ?, ?, strftime('%s', 'now'))",
                   user_id, current_id, stock["price"], amount, "BUY")

        return redirect("/")

    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    user_id = session.get("user_id")

    # Get purchases information ordered by the most recent
    purchases = db.execute("SELECT symbol, action, name, history.amount as amount, price, date FROM history JOIN stocks ON history.stock_id = stocks.id WHERE history.user_id=? ORDER BY date DESC",
                           user_id)

    # Convert date from milliseconds to user friendly date
    for p in purchases:
        date = datetime.fromtimestamp(p["date"])
        p["date"] = date.strftime("%Y-%m-%d %H:%M:%S")

    return render_template("history.html", purchases=purchases)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("auth.html", action="login", title="Log In")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    if request.method == "POST":
        symbol = request.form.get("symbol")

        if not symbol:
            return apology("Invalid Symbol", 400)

        stocks = lookup(symbol)

        if not stocks:
            return apology("Invalid Symbol", 400)

        stocks["price"] = usd(stocks["price"])
        return render_template("quoted.html", quotes=[stocks])
    else:
        return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        password = request.form.get("password")
        confirmation = request.form.get("confirmation")

        # Ensure password was submitted
        if not password:
            return apology("must provide password", 400)

        # Ensure confirmation password was submitted
        if not confirmation:
            return apology("must confirm password", 400)

        if password != confirmation:
            return apology("passwords mismatch", 400)

        hashed_password = generate_password_hash(request.form.get("password"))
        # Add new user to database
        try:
            inserted_id = db.execute("INSERT INTO users (username, hash) VALUES (?, ?)",
                                     request.form.get("username"), hashed_password)
        except:
            return apology("username already exists", 400)

        session["user_id"] = inserted_id

        return redirect("/")
    else:
        return render_template("auth.html", action="register", title="Register")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    user_id = session.get("user_id")

    if request.method == "POST":
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Ensure a symbol was provided
        if not symbol:
            return apology("must provide symbol", 400)

        # Ensure a valid amount of shares was provided
        if not shares or int(shares) < 0:
            return apology("invalid share amount", 400)

        # Fetch the amout of shares the user has for the requested stock
        rows = db.execute("SELECT id, amount FROM stocks WHERE user_id=? AND symbol=?", user_id, symbol)
        purchases = rows[0]

        amount = int(shares)

        if amount > purchases["amount"] or amount < 1:
            return apology("invalid amount of shares", 400)

        stock = lookup(symbol)

        # Make sure the provided symbol exists
        if not stock:
            return apology("invalid symbol", 400)

        total_price = stock["price"] * amount

        # Add sold amount to user's account
        user = db.execute("SELECT cash FROM users WHERE id=?", user_id)
        db.execute("UPDATE users SET cash = ? WHERE id=?", (user[0]["cash"] + total_price), user_id)

        # Update new owned stock amount
        db.execute("UPDATE stocks SET amount = ? WHERE user_id=? AND symbol=?", (purchases["amount"] - amount), user_id, symbol)

        # Register stock purchase
        db.execute("INSERT INTO history (user_id, stock_id, price, amount, action, date) VALUES (?, ?, ?, ?, ?, strftime('%s', 'now'))",
                   user_id, purchases["id"], stock["price"], amount, "SELL")

        return redirect("/")

    else:
        purchases = db.execute("SELECT symbol FROM stocks WHERE user_id=? AND amount > 0", user_id)

        return render_template("sell.html", purchases=purchases)


def errorhandler(e):
    """Handle error"""
    if not isinstance(e, HTTPException):
        e = InternalServerError()
    return apology(e.name, e.code)


# Listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
