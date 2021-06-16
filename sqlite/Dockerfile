FROM python:latest
RUN apt-get update && apt-get install -y postgresql-client
RUN pip install db-to-sqlite==1.4 db-to-sqlite[postgres] psycopg2-binary
WORKDIR /workdir

CMD ["db-to-sqlite"]
